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

#ifndef REALM_CLUSTER_TREE_HPP
#define REALM_CLUSTER_TREE_HPP

#include <realm/obj.hpp>
#include <realm/util/function_ref.hpp>

namespace realm {

class ClusterTree {
public:
    class ConstIterator;
    class Iterator;
    using TraverseFunction = util::FunctionRef<bool(const Cluster*)>;
    using UpdateFunction = util::FunctionRef<void(Cluster*)>;

    ClusterTree(Table* owner, Allocator& alloc);
    static MemRef create_empty_cluster(Allocator& alloc);

    ClusterTree(ClusterTree&&) = default;

    // Disable copying, this is not allowed.
    ClusterTree& operator=(const ClusterTree&) = delete;
    ClusterTree(const ClusterTree&) = delete;

    bool is_attached() const
    {
        return m_root->is_attached();
    }
    Allocator& get_alloc() const
    {
        return m_alloc;
    }
    const Table* get_owner() const
    {
        return m_owner;
    }
    const Spec& get_spec() const;

    void init_from_ref(ref_type ref);
    void init_from_parent();
    bool update_from_parent(size_t old_baseline) noexcept;

    size_t size() const noexcept
    {
        return m_size;
    }
    void clear(CascadeState&);
    void nullify_links(ObjKey, CascadeState&);
    bool is_empty() const noexcept
    {
        return size() == 0;
    }
    int64_t get_last_key_value() const
    {
        return m_root->get_last_key_value();
    }
    MemRef ensure_writeable(ObjKey k)
    {
        return m_root->ensure_writeable(k);
    }
    Array& get_fields_accessor(Array& fallback, MemRef mem) const
    {
        if (m_root->is_leaf()) {
            return *m_root;
        }
        fallback.init_from_mem(mem);
        return fallback;
    }

    uint64_t bump_content_version()
    {
        m_alloc.bump_content_version();
        return m_alloc.get_content_version();
    }
    void bump_storage_version()
    {
        m_alloc.bump_storage_version();
    }
    uint64_t get_content_version() const
    {
        return m_alloc.get_content_version();
    }
    uint64_t get_instance_version() const
    {
        return m_alloc.get_instance_version();
    }
    uint64_t get_storage_version(uint64_t inst_ver) const
    {
        return m_alloc.get_storage_version(inst_ver);
    }
    void insert_column(ColKey col)
    {
        m_root->insert_column(col);
    }
    void remove_column(ColKey col)
    {
        m_root->remove_column(col);
    }

    // Insert entry for object, but do not create and return the object accessor
    void insert_fast(ObjKey k, const FieldValues& init_values, ClusterNode::State& state);
    // Create and return object
    Obj insert(ObjKey k, const FieldValues&);
    // Delete object with given key
    void erase(ObjKey k, CascadeState& state);
    // Check if an object with given key exists
    bool is_valid(ObjKey k) const;
    // Lookup and return read-only object
    ConstObj get(ObjKey k) const;
    // Lookup and return object
    Obj get(ObjKey k);
    // Lookup ContsObj by index
    ConstObj get(size_t ndx) const;
    // Lookup Obj by index
    Obj get(size_t ndx);
    // Get logical index of object identified by k
    size_t get_ndx(ObjKey k) const;
    // Find the leaf containing the requested object
    bool get_leaf(ObjKey key, ClusterNode::IteratorState& state) const noexcept;
    // Visit all leaves and call the supplied function. Stop when function returns true.
    // Not allowed to modify the tree
    bool traverse(TraverseFunction func) const;
    // Visit all leaves and call the supplied function. The function can modify the leaf.
    void update(UpdateFunction func);

    void enumerate_string_column(ColKey col_key);
    void dump_objects()
    {
        m_root->dump_objects(0, "");
    }
    void verify() const;

private:
    friend class ConstObj;
    friend class Obj;
    friend class Cluster;
    friend class ClusterNodeInner;
    Table* m_owner;
    Allocator& m_alloc;
    std::unique_ptr<ClusterNode> m_root;
    size_t m_size = 0;

    void replace_root(std::unique_ptr<ClusterNode> leaf);

    std::unique_ptr<ClusterNode> create_root_from_mem(Allocator& alloc, MemRef mem);
    std::unique_ptr<ClusterNode> create_root_from_ref(Allocator& alloc, ref_type ref)
    {
        return create_root_from_mem(alloc, MemRef{alloc.translate(ref), ref, alloc});
    }
    std::unique_ptr<ClusterNode> get_node(ref_type ref) const;

    size_t get_column_index(StringData col_name) const;
    void remove_all_links(CascadeState&);
};

class ClusterTree::ConstIterator {
public:
    typedef std::output_iterator_tag iterator_category;
    typedef const Obj value_type;
    typedef ptrdiff_t difference_type;
    typedef const Obj* pointer;
    typedef const Obj& reference;

    ConstIterator(const ClusterTree& t, size_t ndx);
    ConstIterator(const ClusterTree& t, ObjKey key);
    ConstIterator(Iterator&&);
    ConstIterator(const ConstIterator& other)
        : ConstIterator(other.m_tree, other.m_key)
    {
    }
    ConstIterator& operator=(ConstIterator&& other)
    {
        REALM_ASSERT(&m_tree == &other.m_tree);
        m_key = other.m_key;
        return *this;
    }
    reference operator*() const
    {
        return *operator->();
    }
    pointer operator->() const;
    ConstIterator& operator++();
    ConstIterator& operator+=(ptrdiff_t adj);
    ConstIterator operator+(ptrdiff_t adj)
    {
        ConstIterator tmp(*this);
        tmp += adj;
        return tmp;
    }
    bool operator!=(const ConstIterator& rhs) const
    {
        return m_key != rhs.m_key;
    }

protected:
    const ClusterTree& m_tree;
    mutable uint64_t m_storage_version = uint64_t(-1);
    mutable Cluster m_leaf;
    mutable ClusterNode::IteratorState m_state;
    mutable uint64_t m_instance_version = uint64_t(-1);
    mutable ObjKey m_key;
    mutable std::aligned_storage<sizeof(Obj), alignof(Obj)>::type m_obj_cache_storage;

    ObjKey load_leaf(ObjKey key) const;
};

class ClusterTree::Iterator : public ClusterTree::ConstIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Obj value_type;
    typedef Obj* pointer;
    typedef Obj& reference;

    Iterator(const ClusterTree& t, size_t ndx)
        : ConstIterator(t, ndx)
    {
    }
    Iterator(const ClusterTree& t, ObjKey key)
        : ConstIterator(t, key)
    {
    }

    reference operator*() const
    {
        return *operator->();
    }
    pointer operator->() const
    {
        return const_cast<pointer>(ConstIterator::operator->());
    }
    Iterator& operator++()
    {
        return static_cast<Iterator&>(ConstIterator::operator++());
    }
    Iterator& operator+=(ptrdiff_t adj)
    {
        return static_cast<Iterator&>(ConstIterator::operator+=(adj));
    }
    Iterator operator+(ptrdiff_t adj)
    {
        Iterator tmp(*this);
        tmp.ConstIterator::operator+=(adj);
        return tmp;
    }
};
}

#endif /* REALM_CLUSTER_TREE_HPP */
