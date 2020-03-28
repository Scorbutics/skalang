#pragma once

#include <sstream>
#include "SerializerNatives.h"

namespace ska {

	class SerializerOutputData {
	public:
		SerializerOutputData(std::stringstream& buffer, NativeContainer& natives) :
			m_buffer(&buffer),
			m_natives(&natives) {
		}
		
		std::stringstream& buffer() { return *m_buffer; }
		NativeContainer& natives() { return *m_natives; }

	private:
		std::stringstream* m_buffer;
		NativeContainer* m_natives;
	};

	template <class ErrorPusher, std::size_t BytesRequired>
	class SerializerSafeZoneGeneric {
	public:
		SerializerSafeZoneGeneric(ErrorPusher& pusher, std::string name, SerializerOutputData& data) :
			m_data(data),
			m_name(std::move(name)),
			m_pusher(pusher) {
		}

		~SerializerSafeZoneGeneric() {
			if (m_bytesCounter != BytesRequired) {
				m_pusher.onError("in zone \"" + m_name + "\", bad serializer sync detected : " + std::to_string(m_bytesCounter) + " bytes were written for " + std::to_string(BytesRequired) + " initially expected");
			}
		}
		
		template <std::size_t bytes>
		SerializerSafeZoneGeneric<ErrorPusher, bytes> acquireMemory(std::string zoneName) {
			static_assert(bytes <= BytesRequired, "Unable to extract so many bytes from the existing safe zone");
			m_bytesCounter += bytes;
			if (m_bytesCounter > BytesRequired) {
				m_pusher.onError("in zone \"" + m_name + " | " + zoneName + "\", tried to acquire " + std::to_string(bytes) + " but only " + std::to_string(m_bytesCounter - bytes) + " were remaining into parent zone \"" + m_name + "\"");
			}
			return SerializerSafeZoneGeneric<ErrorPusher, bytes>{m_pusher, m_name + " | " + zoneName, m_data};
		}

		template <class T>
		void write(const T& data) {
			if constexpr (std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, std::string>) {
				m_bytesCounter += sizeof(int64_t);
				m_data.natives().emplace(data, m_data.natives().size());
				auto refIndex = m_data.natives().at(data);
				m_data.buffer().write(reinterpret_cast<const char*>(&refIndex), sizeof(int64_t));
			} else {
				m_bytesCounter += sizeof(T);
				m_data.buffer().write(reinterpret_cast<const char*>(&data), sizeof(T));
			}
		}

	private:
		SerializerOutputData m_data;
		std::string m_name;
		ErrorPusher& m_pusher;
		std::size_t m_bytesCounter = 0;
	};

	class SerializerOutput {
	public:
		SerializerOutput(SerializerOutputData data) :
			m_data(std::move(data)) {
		}

		template <std::size_t bytes>
		SerializerSafeZoneGeneric<SerializerOutput, bytes> acquireMemory(std::string zoneName) {
			validate();
			return SerializerSafeZoneGeneric<SerializerOutput, bytes>{*this, std::move(zoneName), m_data};
		}

		void onError(std::string errorMsg) {
			m_errors << std::move(errorMsg) << std::endl;
		}

		void validate() {
			auto errorMsg = m_errors.str();
			std::stringstream().swap(m_errors);
			if (!errorMsg.empty()) {
				throw std::runtime_error(std::move(errorMsg));
			}
		}

	private:
		SerializerOutputData m_data;
		std::stringstream m_errors;
	};

	template <std::size_t BytesRequired>
	using SerializerSafeZone = SerializerSafeZoneGeneric<SerializerOutput, BytesRequired>;
}
