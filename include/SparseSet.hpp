#pragma once
#include <vector>
#include <optional>
#include <type_traits>

namespace aZero
{
	namespace DS
	{
		template<typename T>
		concept UnsignedInteger =
			std::is_same_v<T, uint64_t> ||
			std::is_same_v<T, uint32_t> ||
			std::is_same_v<T, uint16_t> ||
			std::is_same_v<T, uint8_t>;

		template<typename T>
		concept IsTriviallyCopyable = std::is_trivially_copyable_v<T>;

		template<UnsignedInteger IDType, IsTriviallyCopyable ElementType>
		class SparseSet
		{
		public:
			static constexpr IDType InvalidIndex{ std::numeric_limits<IDType>::max() };

			SparseSet()
				:m_CurrentLast(0){ }

			SparseSet(IDType numElements)
				:m_CurrentLast(0), m_ID_To_Element(numElements, InvalidIndex){ }

			void Insert(IDType id, const ElementType& element)
			{
				if (!this->Exists(id))
				{
					if (m_CurrentLast < m_Elements.size())
					{
						m_Elements.at(m_CurrentLast) = element;
						m_Element_To_ID.at(m_CurrentLast) = id;
					}
					else
					{
						m_Elements.push_back(element);
						m_Element_To_ID.push_back(id);
					}
					m_ID_To_Element.at(id) = m_CurrentLast;
					m_CurrentLast++;
				}
			}

			bool Insert(IDType id, ElementType&& element)
			{
				if (!this->Exists(id))
				{
					if (m_CurrentLast < m_Elements.size())
					{
						m_Elements.at(m_CurrentLast) = std::move(element);
						m_Element_To_ID.at(m_CurrentLast) = id;
					}
					else
					{
						m_Elements.push_back(std::move(element));
						m_Element_To_ID.push_back(id);
					}
					m_ID_To_Element.at(id) = m_CurrentLast;
					m_CurrentLast++;
					return true;
				}
				return false;
			}

			bool Erase(IDType id)
			{
				if (this->Exists(id))
				{
					const IDType LastIndex = m_CurrentLast - 1;
					const IDType RemovedElementIndex = m_ID_To_Element.at(id);
					if (RemovedElementIndex != LastIndex)
					{
						m_Elements.at(RemovedElementIndex) = std::move(m_Elements.at(LastIndex));
						const IDType LastElementID = m_Element_To_ID.at(LastIndex);
						m_ID_To_Element.at(LastElementID) = RemovedElementIndex;
						m_Element_To_ID.at(RemovedElementIndex) = LastElementID;
					}
					m_ID_To_Element.at(id) = InvalidIndex;
					m_CurrentLast--;
					return true;
				}
				return false;
			}

			[[nodiscard]] ElementType& Get(IDType id)
			{
				const IDType elementIndex = m_ID_To_Element.at(id);
				return m_Elements.at(elementIndex);
			}

			[[nodiscard]] const ElementType& Get(IDType id) const
			{
				const IDType elementIndex = m_ID_To_Element.at(id);
				return m_Elements.at(elementIndex);
			}

			[[nodiscard]] std::optional<std::reference_wrapper<ElementType>> GetIfExists(IDType id)
			{
				if (this->Exists(id))
				{
					return std::optional<std::reference_wrapper<ElementType>>{std::ref(m_Elements.at(m_ID_To_Element.at(id)))};
				}
				else
				{
					return std::nullopt;
				}
			}

			[[nodiscard]] std::optional<std::reference_wrapper<const ElementType>> GetIfExists(IDType id) const
			{
				if (this->Exists(id))
				{
					return std::optional<std::reference_wrapper<const ElementType>>{std::ref(m_Elements.at(m_ID_To_Element.at(id)))};
				}
				else
				{
					return std::nullopt;
				}
			}

			[[nodiscard]] std::vector<ElementType>& GetData() { return m_Elements; }

			[[nodiscard]] const std::vector<ElementType>& GetData() const { return m_Elements; }

			[[nodiscard]] bool Exists(IDType id) const
			{
				if (id >= m_ID_To_Element.size())
					return false;

				const IDType elementIndex = m_ID_To_Element.at(id);
				return elementIndex != InvalidIndex;
			}

			void ShrinkToFit()
			{
				m_Elements.resize(m_CurrentLast);
				m_Elements.shrink_to_fit();
				m_Element_To_ID.resize(m_CurrentLast);
				m_Element_To_ID.shrink_to_fit();
			}

			void Reserve(IDType numEntries)
			{
				if (numEntries > m_ID_To_Element.size())
				{
					m_ID_To_Element.resize(numEntries, InvalidIndex);
				}
			}

			[[nodiscard]] std::size_t NumSupportedElements() const
			{
				return m_ID_To_Element.size();
			}

			[[nodiscard]] std::size_t InternalSize() const
			{
				return m_Elements.capacity();
			}

			[[nodiscard]] IDType GetElementIndex(IDType id) const { return m_ID_To_Element.at(id); }

			private:
				std::vector<IDType> m_ID_To_Element;
				std::vector<IDType> m_Element_To_ID;
				std::vector<ElementType> m_Elements;
				IDType m_CurrentLast;
		};
	}
}