#pragma once

class SysCallMonitorWidget
{
public:
	void OnPaint()
	{
        ImGui::SeparatorText(u8"ѡ��");
        ImGui::Checkbox(u8"����",&active_); ImGui::SameLine();
        ImGui::Checkbox(u8"������־", &save_); ImGui::SameLine();
        ImGui::Button(u8"���");
        ImGui::SeparatorText(u8"���");
        if (ImGui::TreeNode("root"))
        {
            for (auto n : monitor_)
            {
                if (ImGui::TreeNode(n.first.c_str()))
                {
                    for (auto m : n.second)
                    {
                        ImGui::Text("%d : %s", m.second, m.first.c_str());
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
	}

    auto GetActive()
    {
        return active_;
    }

    void  SetActive(bool b)
    {
        active_ = b;
    }

    auto GetSave()
    {
        return save_;
    }

    bool IsExsist(std::string key)
    {
        return monitor_.count(key) > 0;
    }

    void SetValue(std::string k1,std::string k2,uint64_t v,bool inc=false)
    {
        //inc = ture������ֵ;=false������ֵ
        if (inc)
            monitor_[k1][k2] += v;
        else
            monitor_[k1][k2] = v;
    }
private:

    bool active_=false;
    bool save_=false;
    std::map<std::string, std::map<std::string, uint64_t>> monitor_{};
};