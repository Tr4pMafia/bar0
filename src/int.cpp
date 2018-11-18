//
// Bareflank Hypervisor Trap INT3
// Copyright (C) 2018 morimolymoly (Tr4pMafia)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <bfvmm/hve/arch/intel_x64/vcpu.h>
#include <bfvmm/hve/arch/intel_x64/exit_handler.h>
#include <bfvmm/vcpu/vcpu.h>
#include <bfdebug.h>
#include <bfvmm/vcpu/vcpu_factory.h>
#include <bfgsl.h>
#include <bfstring.h>
#include <string>
#include <vector>
#include <bfvmm/memory_manager/arch/x64/unique_map.h>
#include <bfvmm/memory_manager/arch/x64/cr3/mmap.h>
#include <intrinsics.h>

/*
moly@yayoi:~$ sudo cat /proc/iomem
[sudo] password for moly: 
00000000-00000fff : Reserved
...
  fa000000-fb0fffff : PCI Bus 0000:02
    fa000000-faffffff : 0000:02:00.0
    fb080000-fb083fff : 0000:02:00.1
      fb080000-fb083fff : ICH HD audio
  fb100000-fb1fffff : PCI Bus 0000:06
    fb100000-fb13ffff : 0000:06:00.0
    fb140000-fb14ffff : 0000:06:00.0
      fb140000-fb14ffff : tg3
  fb200000-fb2fffff : PCI Bus 0000:05
    fb200000-fb201fff : 0000:05:00.0
      fb200000-fb201fff : xhci-hcd
  fb300000-fb31ffff : 0000:00:19.0
    fb300000-fb31ffff : e1000e
...
*/

#define E1000E_BAR0    0xfb300000    // Guest Physical Address
#define ICH_AUDIO_BAR0 0xfb080000    // Guest Physical Address

namespace mafia
{
namespace intel_x64
{
std::once_flag flag{};
class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : bfvmm::intel_x64::vcpu{id}
    {
        std::call_once(flag, [&] {
            auto audio = bfvmm::x64::make_unique_map<uint32_t>(ICH_AUDIO_BAR0,
            bfvmm::x64::cr3::mmap::memory_type::uncacheable);
            auto audio_pointer = audio.get();
            //bfdebug_nhex(0, "audio mmio unique gm m physint", g_mm->virtptr_to_physint(&audio_pointer[0]));
            bfdebug_nhex(0, "audio mmio regs", audio_pointer);
            
            // TODO: gsl::span to wrap it
            bfdebug_nhex(0, "audio mmio regs", audio_pointer[0]);


            /*
            moly@yayoi:~/lab$ sudo ./mem fb080000 100 | hexdump -x
            0000000    4405    0100    003c    001d    0101    0000    0000    0000
            0000010    0000    0000    0000    0000    0000    0000    0000    0000
            0000020    0000    c000    0000    0000    0000    0000    0000    0000
            0000030    3482    9266    0000    0000    0000    0000    0000    0000
            0000040    5000    5642    0004    0000    0037    0037    0002    0042
            0000050    5800    5642    0004    0000    0037    0001    0003    0042
            0000060    0000    0000    0000    0000    0002    0000    0000    0000
            0000070    4001    5642    0004    0000    0000    0000    0000    0000
            0000080    0000    0000    0000    0000    0000    0000    0000    0000
            0000090    0028    0000    0000    0000    0000    0000    0000    0000
            00000a0    0000    0000    0000    0000    0000    0000    0000    0000
            00000b0    0028    0000    0000    0000    0000    0000    0000    0000
            00000c0    0000    0000    0000    0000    0000    0000    0000    0000
            00000d0    0028    0000    0000    0000    0000    0000    0000    0000
            00000e0    0000    0000    0000    0000    0000    0000    0000    0000
            */
        });
    }
    ~mafia_vcpu() = default;
};
}
}

namespace bfvmm
{
std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

