/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MemoryAllocation
 */

template<kF::GPU::MemoryBindable Bindable, std::input_iterator Iterator>
inline kF::GPU::MemoryAllocation kF::GPU::MemoryAllocation::MakeStaging(const Bindable &toBind,
        const Iterator dataBegin, const Iterator dataEnd) noexcept
{
    MemoryAllocation allocation(MemoryAllocationModel::MakeStaging(toBind));

    allocation.memoryMap(dataBegin, dataEnd);
    return allocation;
}

template<std::input_iterator Iterator>
inline void kF::GPU::MemoryAllocation::memoryMap(const Iterator dataBegin, const Iterator dataEnd) noexcept
{
    using Value = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;

    auto &allocator = parent().memoryAllocator();
    auto mapped = allocator.beginMemoryMap<Value>(*this);
    std::uninitialized_copy(dataBegin, dataEnd, mapped);
    allocator.endMemoryMap(*this);
}