/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISP_MAKE_UNIQUE_H
#define DISP_MAKE_UNIQUE_H

#include <memory>
#include <utility>

namespace OHOS {
namespace HDI {
namespace DISPLAY {
/**
 * @brief C++11 兼容的 std::make_unique 等价实现。
 *
 * 本工程以 C++11 为编译基线，标准库未提供 std::make_unique（C++14 引入），
 * 而编码规范 G.RES.09-CPP 要求创建 std::unique_ptr 时不得在调用处直接使用 new。
 * 这里以可变参数模板与完美转发出同等语义：把唯一的一次内存分配收敛在本函数内部，
 * 调用处不再出现裸 new，同时不引入任何 C++14 及以上特性。
 *
 * @param args 转发给 T 构造函数的参数
 * @return 持有新创建对象的 std::unique_ptr<T>
 */
template<typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args &&... args)
{
    // 先完成分配与构造，再移交所有权：构造过程抛出异常时由编译器自动回收内存，不会泄漏；
    // std::unique_ptr 接管指针的构造函数不会抛出，故此处不存在所有权空窗期。
    T *object = new T(std::forward<Args>(args)...);
    return std::unique_ptr<T>(object);
}
} // namespace DISPLAY
} // namespace HDI
} // namespace OHOS

#endif // DISP_MAKE_UNIQUE_H
