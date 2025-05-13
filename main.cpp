#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

#include <FontAwesome5.h>
#include <Gothic.h>
#include <time.h>
#include <ImWrap.h>
#include <algorithm>
#include <memory>
#include "Drawing.h"
#include "PakarSystem.h"
#include <unordered_set>
#include <ImNotify.h>



std::unique_ptr<Drawing>            g_drawing;
std::unique_ptr<PakarSystem>        g_pakarSystem;

// Main code

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    g_drawing = std::make_unique<Drawing>();
    if (g_drawing) {
        g_drawing->InitDrawing();
    }
    g_pakarSystem = std::make_unique<PakarSystem>();

    bool done = false;
    while (!done)
    {
        g_drawing->HandleMessage(done);

        g_drawing->OnStartFrame();
        {
            g_drawing->RenderHook();
            {
                static std::vector<bool> selectedGejala(g_pakarSystem->GetNamaGejala().size(), false);
                static std::vector<uint8_t> hasilKemungkinanDiagnosa;
                static std::vector<uint8_t> hasilDiagnosaPalingAkurat;
                static std::vector<std::pair<uint8_t, size_t>> persentaseDiagnosa;
                static ImGuiTextFilter m_FilterPakar, m_FilterBuku;
                static bool isShowBukuPenyakit{ false };
                ImGuiIO& io = ImGui::GetIO();

               
                ImGui::Begin("Pakar System", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
                {
                    ImGui::BeginChild("##OUTER_CHILD_01", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, ImGui::GetFrameHeightWithSpacing() * -1), true, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
                    {
                        ImGui::Text(ICON_FA_LIST " Pilih Gejala:");
                        ImGui::Separator();
                        ImGui::BeginChild("##INNER_CHILD_01", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing() * -1), false);
                        {

                            if (ImGui::BeginTable("GejalaTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
                            {
                                ImGui::TableSetupColumn("No", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                                ImGui::TableSetupColumn("Nama Gejala", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableSetupColumn("Pilih", ImGuiTableColumnFlags_WidthFixed, 60.0f);

                                ImGui::TableHeadersRow();

                                for (size_t i = 0; i < selectedGejala.size(); ++i)
                                {
                                    if (!m_FilterPakar.PassFilter(g_pakarSystem->GetNamaGejala()[i].c_str()))
                                        continue;

                                    ImGui::TableNextRow();
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::Text("%zu", i + 1);
                                    ImGui::TableSetColumnIndex(1);
                                    ImGui::TextUnformatted(g_pakarSystem->GetNamaGejala()[i].c_str());

                                    ImGui::TableSetColumnIndex(2);
                                    bool selected = selectedGejala[i];
                                    ImGui::PushID(static_cast<int>(i));
                                    if (ImGui::Checkbox("##checkbox", &selected))
                                    {
                                        selectedGejala[i] = selected;
                                    }
                                    ImGui::PopID();
                                }

                                ImGui::EndTable();
                            }

                        }
                        ImGui::EndChild();
                        ImGui::Separator();
                        m_FilterPakar.DrawWithHint("##FilterGejala", "Filter Gejala", ImGui::GetContentRegionAvail().x);

                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("##OUTER_CHILD_02", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing() * -1), true);
                    {
                        ImGui::Text(ICON_FA_AMBULANCE " Kemungkinan Penyakit:");
                        ImGui::BeginChild("##INNER_CHILD_03", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.5f), true);
                        {
                            if (!persentaseDiagnosa.empty())
                            {
                                if (ImGui::BeginTable("##TableDiagnosa", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                                {
                                    ImGui::TableSetupColumn("Nama Penyakit");
                                    ImGui::TableSetupColumn("Persentase");
                                    ImGui::TableHeadersRow();

                                    for (const auto& [id, persen] : persentaseDiagnosa)
                                    {
                                        ImGui::TableNextRow();

                                        ImGui::TableSetColumnIndex(0);
                                        ImGui::TextWrapped("%s", g_pakarSystem->GetNamaPenyakit()[id].c_str());

                                        ImGui::TableSetColumnIndex(1);
                                        ImGui::Text("%zu%%", persen);
                                    }

                                    ImGui::EndTable();
                                }
                            }
                            else
                            {
                                ImGui::TextWrapped("Tidak ada penyakit yang cocok dengan gejala yang dipilih.");
                            }
                        }
                        ImGui::EndChild();
                        ImGui::Spacing();
                        ImGui::TextColored(ImGui::rbw, ICON_FA_BULLSEYE " Diagnosa Paling Akurat:");
                        ImGui::BeginChild("##INNER_CHILD_04", ImVec2(0, 0), true);
                        {

                            if (!hasilDiagnosaPalingAkurat.empty())
                            {
                                for (uint8_t id : hasilDiagnosaPalingAkurat)
                                    ImGui::BulletText("%s", g_pakarSystem->GetNamaPenyakit()[id].c_str());
                            }
                            else
                            {
                                ImGui::TextWrapped("Tidak ada penyakit yang cocok dengan gejala yang dipilih.");
                            }
                        }
                        ImGui::EndChild();
                    }
                    ImGui::EndChild();

                    if (ImGui::Button(ICON_FA_SEARCH " Diagnosa", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 0)))
                    {
                        std::vector<uint8_t> inputGejala;
                        inputGejala.reserve(selectedGejala.size());
                        for (size_t i = 0; i < selectedGejala.size(); ++i)
                            if (selectedGejala[i]) inputGejala.push_back(static_cast<uint8_t>(i));
                        g_pakarSystem->Diagnosa(inputGejala, hasilKemungkinanDiagnosa, hasilDiagnosaPalingAkurat, persentaseDiagnosa);

                        if(hasilDiagnosaPalingAkurat.empty() && hasilKemungkinanDiagnosa.empty())
                            ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Tidak ada penyakit yang cocok dengan kriteria yang diberikan!" });
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_BOOK " Buku penyakit", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                    {
                        ImGui::OpenPopup("Buku Penyakit");
                    }

                }
                {
                    ImGui::SetNextWindowSize(ImVec2(800.0f, 500.0f), ImGuiCond_::ImGuiCond_Once);
                    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
                    ImVec2 window_size = ImVec2(800.0f, 500.0f);
                    ImGui::SetNextWindowPos(ImVec2(center.x - window_size.x * 0.5f, center.y - window_size.y * 0.5f), ImGuiCond_Once);
                    if (ImGui::BeginPopupModal("Buku Penyakit"))
                    {

                        static int selectedPenyakitID = -1;

                        ImGui::BeginChild("##LIST_PENYAKIT", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetFrameHeightWithSpacing() * -1), true);
                        {

                            ImGui::Text(ICON_FA_LIST " Daftar Penyakit");
                            ImGui::BeginChild("##LIST_PENYAKIT_INNER", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing() * -1), true);

                            {
                                for (const auto& [penyakitID, gejalaList] : g_pakarSystem->GetGejalaPenyakit())
                                {
                                    const std::string nama = g_pakarSystem->GetNamaPenyakit()[penyakitID];
                                    if (!m_FilterBuku.PassFilter(nama.c_str()))
                                        continue;
                                    ImGui::PushID(nama.c_str());
                                    if (ImGui::Selectable(nama.c_str(), selectedPenyakitID == penyakitID))
                                    {
                                        selectedPenyakitID = penyakitID;
                                    }
                                    ImGui::PopID();
                                }
                            }
                            ImGui::EndChild();
                            m_FilterBuku.DrawWithHint("##Filter_Buku", "Filter Penyakit", ImGui::GetContentRegionAvail().x);
                        }
                        ImGui::EndChild();
                        ImGui::SameLine();

                        ImGui::BeginChild("##DETAIL_GEJALA", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing() * -1), true);
                        {
                            if (selectedPenyakitID >= 0) {

                                const std::string nama = g_pakarSystem->GetNamaPenyakit()[selectedPenyakitID];
                                ImGui::TextWrapped(ICON_FA_BUG" Gejala dari %s", nama.c_str());
                            }
                            ImGui::BeginChild("##LIST_PENYAKIT_INNER", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
                            {
                                if (selectedPenyakitID >= 0)
                                {


                                    const auto gejalaList = g_pakarSystem->GetGejalaPenyakit().at(selectedPenyakitID);
                                    for (uint8_t gid : gejalaList)
                                    {
                                        if (gid < g_pakarSystem->GetNamaGejala().size())
                                        {
                                            const std::string gejala = g_pakarSystem->GetNamaGejala()[gid];
                                            ImGui::BulletText("%s", gejala.c_str());
                                        }
                                    }
                                }
                                else
                                {
                                    ImGui::TextWrapped("Pilih salah satu penyakit untuk melihat gejalanya.");
                                }
                            }
                            ImGui::EndChild();

                        }
                        ImGui::EndChild();
                        if (ImGui::Button("Tutup Buku", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                
                ImGui::End();
                
            }
        }
        g_drawing->OnEndFrame();
    }
    g_drawing->OnCleanUp();

    return 0;
}




