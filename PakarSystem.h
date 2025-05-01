#pragma once
#include <unordered_map>
#include <vector>
#include <stdint.h>
#include <string>
class PakarSystem {
	private:
		std::vector<std::string> m_NamaGejala;
		std::vector<std::string> m_NamaPenyakit;
		std::unordered_map<uint8_t, std::vector<uint8_t>> m_GejalaPenyakit;
	public:
		PakarSystem();
		void InitNamaGejala(), InitNamaPenyakit(), InitGejalaPenyakit();


		[[nodiscard]] std::vector<std::string> GetNamaGejala() const;
		[[nodiscard]] std::vector<std::string> GetNamaPenyakit() const;
		[[nodiscard]] std::unordered_map<uint8_t, std::vector<uint8_t>> GetGejalaPenyakit() const;
		[[nodiscard]] std::vector<uint8_t> MinusOne(std::vector<uint8_t> input_vector) const;
		void Diagnosa(const std::vector<uint8_t>& inputGejala,
			std::vector<uint8_t>& kemungkinan,
			std::vector<uint8_t>& akurat) const;
};