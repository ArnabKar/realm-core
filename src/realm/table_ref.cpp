/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#include <realm/table_ref.hpp>
#include <realm/table.hpp>

namespace realm {

ConstTableRef::ConstTableRef(const Table* t_ptr)
    : m_table(const_cast<Table*>(t_ptr))
    , m_instance_version(t_ptr ? t_ptr->get_instance_version() : 0)
{
}

ConstTableRef::operator bool() const
{
    return m_table != nullptr && m_table->get_instance_version() == m_instance_version;
}


const Table* ConstTableRef::operator->() const
{
    if (!operator bool()) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return m_table;
}

Table* TableRef::operator->() const
{
    if (!operator bool()) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return m_table;
}

const Table* ConstTableRef::checked() const
{
    if (m_table == nullptr) {
        throw realm::NoSuchTable();
    }
    if (m_table->get_instance_version() != m_instance_version) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return m_table;
}

Table* TableRef::checked() const
{
    if (m_table == nullptr) {
        throw realm::NoSuchTable();
    }
    if (m_table->get_instance_version() != m_instance_version) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return m_table;
}

const Table* ConstTableRef::checked_or_null() const
{
    if (m_table == nullptr) {
        return m_table;
    }
    if (m_table->get_instance_version() != m_instance_version) {
        return nullptr;
    }
    return m_table;
}

Table* TableRef::checked_or_null() const
{
    if (m_table == nullptr) {
        return m_table;
    }
    if (m_table->get_instance_version() != m_instance_version) {
        return nullptr;
    }
    return m_table;
}

const Table& ConstTableRef::operator*() const
{
    if (!operator bool()) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return *m_table;
}

Table& TableRef::operator*() const
{
    if (!operator bool()) {
        throw realm::LogicError(LogicError::detached_accessor);
    }
    return *m_table;
}


}
