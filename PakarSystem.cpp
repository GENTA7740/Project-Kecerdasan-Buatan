#include "PakarSystem.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <unordered_set>
#include <ImNotify.h>
PakarSystem::PakarSystem()
{
	this->InitNamaGejala();
	this->InitNamaPenyakit();
	this->InitGejalaPenyakit();
}



void PakarSystem::InitNamaGejala()
{
	this->m_NamaGejala.assign({
		"Demam",
		"Sakit kepala",
		"Nyeri saat bicara atau menelan",
		"Batuk",
		"Hidung tersumbat",
		"Nyeri telinga",
		"Nyeri tenggorokan",
		"Hidung meler",
		"Letih dan lesu",
		"Mual dan muntah",
		"Selaput lendir merah dan bengkak",
		"Ada benjolan di leher",
		"Nyeri leher",
		"Pembengkakan kelenjar getah bening",
		"Pendarahan hidung",
		"Suara serak",
		"Bola mata bergerak tanpa sadar",
		"Dahi sakit",
		"Leher bengkak",
		"Tuli",
		"Ada yang tumbuh di mulut",
		"Air liur menetes",
		"Berat badan turun",
		"Bunyi napas abnormal",
		"Infeksi sinus",
		"Nyeri antara mata",
		"Nyeri pinggir hidung",
		"Nyeri pipi di bawah mata",
		"Nyeri wajah",
		"Perubahan kulit",
		"Perubahan suara",
		"Radang gendang telinga",
		"Sakit gigi",
		"Serangan vertigo",
		"Telinga berdenging",
		"Telinga terasa penuh",
		"Tenggorokan gatal",
		"Tubuh tak seimbang"
	});

}
void PakarSystem::InitNamaPenyakit()
{
	this->m_NamaPenyakit.assign({
		"CONTRACT ULCERS",
		"ABAES PARAFARINGEAL",
		"ABAES PERITONAILER",
		"BAROTITIS MEDIA",
		"DEVIASI SEPTUM",
		"FARINGITIS",
		"KANKER LARING",
		"KANKER LEHER DAN KEPALA",
		"KANKER LEHER METASTATIK",
		"KANKER NASOFARING",
		"KANKER TONSIL",
		"LARINGITIS",
		"NEURONITIS VESTIBULARIS",
		"OSTEOSKLEROSIS",
		"OTITIS MEDIA AKUT",
		"MENIERE",
		"TONSILITIS",
		"TUMOR SYARAF PENDENGARAN",
		"VERTIGO POSTULAR",
		"SINUSITIS MAKSILARIS",
		"SINUSITIS FRONTALIS",
		"SINUSITIS ETMOIDALIS",
		"SINUSITIS SFENOIDALIS",
		"PERUT"
	});
}

void PakarSystem::InitGejalaPenyakit()
{
	this->m_GejalaPenyakit.insert({
		{ 0, this->MinusOne({3, 16}) },
		{ 1, this->MinusOne({3, 19}) },
		{ 2, this->MinusOne({1, 2, 7, 14, 16, 22}) },
		{ 3, this->MinusOne({2, 6}) },
		{ 4, this->MinusOne({1, 5, 6, 15, 25, 29}) },
		{ 5, this->MinusOne({1, 3, 7, 13, 14}) },
		{ 6, this->MinusOne({3, 4, 7, 13, 16, 23, 24}) },
		{ 7, this->MinusOne({3, 12, 15, 21, 30, 31}) },
		{ 8, this->MinusOne({12}) },
		{ 9, this->MinusOne({5, 15}) },
		{ 10, this->MinusOne({7, 12}) },
		{ 11, this->MinusOne({1, 3, 14, 19, 37}) },
		{ 12, this->MinusOne({10, 17}) },
		{ 13, this->MinusOne({20, 35}) },
		{ 14, this->MinusOne({1, 6, 10, 32}) },
		{ 15, this->MinusOne({6, 10, 34, 36}) },
		{ 16, this->MinusOne({1, 2, 3, 4, 7, 10}) },
		{ 17, this->MinusOne({2, 20, 38}) },
		{ 18, this->MinusOne({17}) },
		{ 19, this->MinusOne({1, 2, 4, 5, 8, 9, 11, 28, 33}) },
		{ 20, this->MinusOne({1, 2, 4, 5, 8, 9, 11, 18}) },
		{ 21, this->MinusOne({1, 2, 4, 5, 8, 9, 11, 18, 26, 27}) },
		{ 22, this->MinusOne({1, 2, 4, 5, 6, 8, 9, 11, 12}) },
		{ 23, this->MinusOne({1, 2, 3, 4}) },
	});
}

std::vector<std::string> PakarSystem::GetNamaGejala() const
{
	return this->m_NamaGejala;
}

std::vector<std::string> PakarSystem::GetNamaPenyakit() const
{
	return this->m_NamaPenyakit;
}

std::unordered_map<uint8_t, std::vector<uint8_t>> PakarSystem::GetGejalaPenyakit() const
{
	return this->m_GejalaPenyakit;
}

std::vector<uint8_t> PakarSystem::MinusOne(std::vector<uint8_t> input_vector) const
{
	std::vector<uint8_t> result;
	result.reserve(input_vector.size());

	for (uint8_t val : input_vector) 
		result.push_back(val > 0 ? val - 1 : 0);

	return result;
}

void PakarSystem::Diagnosa(const std::vector<uint8_t>& inputGejala, std::vector<uint8_t>& kemungkinan, std::vector<uint8_t>& akurat, std::vector<std::pair<uint8_t, size_t>>& persentaseDiagnosa) const
{
	kemungkinan.clear();
	akurat.clear();
	persentaseDiagnosa.clear();

	std::unordered_set<uint8_t> inputSet(inputGejala.begin(), inputGejala.end());
	size_t maxCocok = 0;

	for (const auto& [id, gejala] : this->GetGejalaPenyakit())
	{
		size_t cocok = 0;
		for (uint8_t g : gejala)
			cocok += inputSet.count(g);

		if (cocok)
		{
			kemungkinan.push_back(id);
			persentaseDiagnosa.emplace_back(id, static_cast<size_t>((float)cocok / gejala.size() * 100.0f));
			maxCocok = std::max(maxCocok, cocok);
		}

		if (cocok == gejala.size() && inputSet.size() == gejala.size())
		{
			std::vector<uint8_t> a = gejala, b = inputGejala;
			std::sort(a.begin(), a.end());
			std::sort(b.begin(), b.end());
			if (a == b)
			{
				akurat.clear();
				akurat.push_back(id);
			}
		}
	}
}
