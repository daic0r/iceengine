#include <GUI/ListBox.h>
#include <GUI/StackPanel.h>
#include <GUI/ScrollWidget.h>
#include <GUI/TextLabel.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <GUI/WidgetManager.h>
#include <System/SystemServices.h>
#include <algorithm>

namespace Ice::GUI
{
    // Requires: setting my constraints in the func during creation before adding to parent
    ListBox::ListBox(std::initializer_list<ColumnInfoType> lNames) {
        setIsConstructing(true);
        //setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>())
        if (lNames.size() > 0)
            setColumns(lNames);

        auto& wMan = systemServices.getWidgetManager();
        setIsConstructing(true);
        addPresetConstraints(ConstraintPreset::FILL_PARENT, this);
        setElementSpacing(.01f);
        setDefaultVerticalAlignment(VerticalAlignment::TOP);

        if (lNames.size() > 0) {
            addRow([&lNames,&wMan,this](Widget* pParent) {
                std::vector<Widget*> vHeaderColumns;
                vHeaderColumns.resize(lNames.size());
                std::size_t nIndex{};
                for (auto& pHeader : vHeaderColumns) {
                    auto pText = wMan.createWidgetWithFunc<TextLabel>([this, nIndex](TextLabel* pText) {
                        //pText->setSizeConstraint<DirectSizeConstraint<Percent>>(OrthogonalAxis::HORIZONTAL, (*m_vColumns)[nIndex].second);
                    }, pParent, m_vColumns.has_value() ? (*m_vColumns)[nIndex].first : "");
                    auto st = pText->style();
                    st.fontName() = "Segoe UI";
                    pText->setStyle(st);
                    ++nIndex;
                }
                return vHeaderColumns;
            });
        }

        auto pScroller = wMan.createWidget<ScrollWidget>(this);
        pScroller->setHorizontalScrollBar(false);

        auto pContent = wMan.createWidget<VerticalStackPanel>(pScroller);
        pContent->setSizeConstraint<DirectSizeConstraint<Percent>>(OrthogonalAxis::HORIZONTAL, 100.0_pct);

        m_pContent = pContent;

        setIsConstructing(false);
    }

    const ListBox::ColumnInfoType& ListBox::column(std::size_t n) const {
        if (!m_vColumns.has_value())
            throw std::runtime_error("List box has no defined columns!");

        return m_vColumns->at(n);
    }

    void ListBox::setColumns(std::initializer_list<ColumnInfoType> lNames) {
        if (!m_vColumns.has_value())
            m_vColumns = std::vector<ColumnInfoType>{};
        std::copy(lNames.begin(), lNames.end(), std::back_inserter(*m_vColumns));
    }

    void ListBox::addRow(const std::function<std::vector<Widget*>(Widget*)>& makeRowFunc) {
        auto& wMan = systemServices.getWidgetManager();

        auto pRowStack = wMan.createWidgetWithFunc<HorizontalStackPanel>([this](Widget* pRowStack) {
            pRowStack->setSizeConstraint<DirectSizeConstraint<Pixels>>(OrthogonalAxis::VERTICAL, m_vRows.size() == 0 ? headerHeight() : rowHeight());
        }, m_vRows.size() == 0 ? static_cast<Widget*>(this) : static_cast<Widget*>(m_pContent));
        
        pRowStack->setIsConstructing(true);

        auto vRet = makeRowFunc(pRowStack);
        if ((!m_vColumns.has_value() && vRet.size() != 1) || (m_vColumns.has_value() && vRet.size() != m_vColumns->size()))
            throw std::runtime_error("Number of widgets returned does not match number of columns for list box!");
        int nColIdx{};
        for (auto pWidget : vRet) {
            if (pWidget) {
                pWidget->setBorderThickness(1_px);
                pWidget->setSizeConstraint<GUI::DirectSizeConstraint<Percent>>(OrthogonalAxis::HORIZONTAL, column(nColIdx).second);
            }
            ++nColIdx;
        }
        m_vRows.push_back(pRowStack);

        pRowStack->setIsConstructing(false);
    }

    void ListBox::deleteRow(std::size_t n) {
        auto& wMan = systemServices.getWidgetManager();

        wMan.deleteWidget(m_vRows.at(n)->handle());
        m_vRows.erase(m_vRows.begin() + n);
    }

    void ListBox::deleteRow(HorizontalStackPanel* w) {
        auto iter = std::ranges::find(m_vRows, w);
        if (iter != m_vRows.end())
            deleteRow(std::distance(m_vRows.begin(), iter));
    }
    
} 