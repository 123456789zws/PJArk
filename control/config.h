#pragma

namespace config
{
    namespace global
    {
        plugin plugin_;
        ProcessItem targetProcess;
        ProcessItem injectProcess;
        uint64_t lpPluginDispatch;
    }
    namespace process
    {
        namespace thread
        {
            bool bShow;
            uint32_t pid;
        }
        namespace veh
        {
            bool bShow;
            uint32_t pid;
        }
    }
    namespace syscall
    {
        std::map<std::string, std::map<std::string, uint64_t>> monitor{};
        bool active;
        bool save;
    }
    namespace dbg
    {
        typedef struct _TDR_SET
        {
            bool active;        //�Ƿ�����
            int statue;         //�Ƿ����/�Ƴ�
            int type;           //�ϵ�����:��ִ�С���д��д
            int size;           //�ϵ��С
            char addr[50];      //�ϵ��ַ
        }DrSet;

        int curtChoose = 0;
        DrSet Dr[4];
    }
}
