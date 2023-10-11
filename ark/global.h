#pragma once
#include <stdint.h>
#include "plugin/plugin.h"

namespace global
{
    namespace plugin
    {
        static ::plugin plugin_{};
        static uint64_t lpPluginDispatch{};
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

        static int curtChoose{};
        static DrSet Dr[4]{};
    }
}
