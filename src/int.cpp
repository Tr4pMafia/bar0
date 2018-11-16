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
#include <bfvmm/vcpu/vcpu.h>
#include <bfdebug.h>
#include <bfvmm/vcpu/vcpu_factory.h>
#include <bfgsl.h>
#include <bfstring.h>
#include <string>
#include <vector>
#include <bfvmm/memory_manager/arch/x64/unique_map.h>

#define E1000E_BAR0 0xfb300000

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
            auto phys_range_1 = std::make_pair((uintptr_t)E1000E_BAR0, (size_t)4096);
            auto list = {phys_range_1};
            auto beef = bfvmm::x64::make_unique_map<uint32_t>(list);
            auto ptr = beef.get();
            bfdebug_nhex(0, "address", ptr);
            //bfdebug_nhex(0, "phy", g_mm->physint_to_virtptr((uintptr_t)E1000E_BAR0));
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

