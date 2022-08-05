#ifndef ListBox_h
#define ListBox_h

#include <vector>
#include <string>
#include <optional>
#include <System/Pixels.h>
#include <GUI/StackPanel.h>

namespace Ice::GUI
{
    class ScrollWidget;

    class ListBox : public VerticalStackPanel {
        using ColumnInfoType = std::pair<const char*, Percent>;

        std::optional<std::vector<ColumnInfoType>> m_vColumns;
        Pixels m_headerHeight = 20_px;
        Pixels m_rowHeight = 30_px;
        Ice::GUI::VerticalStackPanel* m_pContent{};
        std::vector<HorizontalStackPanel*> m_vRows;

    public:
        ListBox(std::initializer_list<ColumnInfoType> lNames = {});

        auto columnCount() const noexcept { return m_vColumns.has_value() ? m_vColumns->size() : 1U; }
        void setColumns(std::initializer_list<ColumnInfoType>);

        void addRow(const std::function<std::vector<Widget*>(Widget*)>& makeRowFunc);
        void deleteRow(std::size_t n);
        void deleteRow(HorizontalStackPanel*);

        void setHeaderHeight(Pixels p) noexcept { m_headerHeight = p; }
        auto headerHeight() const noexcept { return m_headerHeight; }

        void setRowHeight(Pixels p) noexcept { m_rowHeight = p; }
        auto rowHeight() const noexcept { return m_rowHeight; }

        auto row(std::size_t n) const { return m_vRows.at(n); }
        decltype(auto) rows() const noexcept { return (m_vRows); }
        auto numRows() const noexcept { return m_vRows.size(); }
        const ColumnInfoType& column(std::size_t n) const;
    };
    
} // namespace Ice


#endif