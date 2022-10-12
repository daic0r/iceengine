//
//  Widget.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Widget_h
#define Widget_h

#include <IceEngine.h>
#include <System/Extents.h>
#include <GUI/Constraints/UIConstraints.h>
#include <GUI/Padding.h>
#include <System/Types.h>

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <map>
#include <vector>
#include <functional>
#include <optional>

namespace Ice {

class IWidgetRenderer;

namespace GUI {

using WidgetHandle = std::uint16_t;
using WidgetContainer = std::map<WidgetHandle, std::unique_ptr<Widget>>;

class  Widget {
    friend class WidgetManager;
    friend class WidgetView;

public:
    static inline glm::vec4 COLOR_DEFAULT{ 0.1f, 0.1f, 0.1f, 0.8f };

    enum class AnimationTarget {
        X,
        Y,
        WIDTH,
        HEIGHT
    };

    enum class ConstraintPreset {
        FILL_PARENT
    };
    
    struct sChildInfo {
        Widget* pChild; // child
        size_t nDrawOrder;  // , draw order (bigger is later)

        auto child() const noexcept { return pChild; }
    };

protected:
    struct sTextInputInfo {
        wchar_t ch;
        bool bBackspace{ false };
        bool bSpace{ false };
        bool bEnter{ false };
        bool bEscape{ false };
    };

private:

    static inline WidgetHandle nNextWidgetHandle{ 1 };

    WidgetHandle m_handle;
    glm::vec4 m_color{ 0.1f, 0.1f, 0.1f, 0.8f };
    glm::vec4 m_foregroundColor{ 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 m_borderColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    Pixels m_borderThickness{ 0 };
    Pixels m_cornerRadius{ 0 };

    std::vector<sChildInfo> m_vChildren;

    Widget* m_pParent{ nullptr };

    std::pair<HorizontalAlignment, std::unique_ptr<PointConstraint>> m_pX;   // what anchor does the constraint refer to? left, center or bottom
    std::pair<VerticalAlignment, std::unique_ptr<PointConstraint>> m_pY; // what anchor does the constraint refer to? bottom, middle or top
    std::unique_ptr<SizeConstraint> m_pWidth;
    std::unique_ptr<SizeConstraint> m_pHeight;
    std::function<Padding(const Widget*)> m_paddingFunc;
    std::function<std::tuple<float, float, float, float>(const Widget*)> m_clipFrameFunc;

    mutable std::optional<float> m_fWidthCached;
    mutable std::optional<float> m_fHeightCached;
    mutable std::optional<float> m_fXCached;
    mutable std::optional<float> m_fYCached;
    mutable std::optional<std::tuple<float, float, float, float>> m_clipFrameCached, m_extentsCached;
    mutable bool m_bConstraintsDirty{ false };
    mutable std::optional<bool> m_bVisible;

    // Interaction
    std::function<void(Widget*, float, float, float, float, std::uint8_t, bool&)> m_mouseOverFunc;
    std::function<void(Widget*, bool&)> m_mouseEnterFunc;
    std::function<void(Widget*, bool&)> m_mouseLeaveFunc;
    std::function<void(Widget*, MouseButton, bool, bool&)> m_mouseButtonFunc;
    std::function<void(Widget*)> m_focusLostFunc;
    std::function<void(Widget*)> m_focusReceivedFunc;
    std::function<void(Widget*, int, int)> m_windowResizedFunc;
    std::function<void(Widget*, const sTextInputInfo&)> m_textInputFunc;

    // Animation
    std::vector<std::function<bool(Widget*, float)>> m_vAnimations;

    bool m_bIsMouseOver{ false };
    bool m_bGraphicsDirty{ false };
    bool m_bHasFocus{ false };
    bool m_bIsTabStoppable{ false };
    bool m_bIsConstructing{};

    static void _subSetConstraintsDirty(bool b, Widget* w, bool bPropagate) noexcept;

public:    

    Widget();
    Widget(Widget&&) = default;
    Widget& operator=(Widget&&) = default;

    // graphicsDirty is intended to be an indicator that the renderer has to refresh
    // it's data (e.g. the text renderer must create a new VAO if the text has changed)
    // for mere changes of size or added children this is not necessary since dimensions
    // are fetched on each render pass anyway.
    // for changes in dimension refreshConstraints() must be called
    bool graphicsDirty() const noexcept { return m_bGraphicsDirty; }
    void setGraphicsDirty(bool b) noexcept { m_bGraphicsDirty = b; }

    bool constraintsDirty() const noexcept { return m_bConstraintsDirty; }
    void setConstraintsDirty(bool b, bool bPropagate = false) noexcept { _subSetConstraintsDirty(b, this, bPropagate); }

    bool visible(bool bRefresh = false) const noexcept;
    void setVisible(bool b) noexcept { m_bVisible = b; }
    
    void setIsConstructing(bool b) noexcept { m_bIsConstructing = b; }
    bool isConstructing() const noexcept { return m_bIsConstructing; }

    WidgetHandle handle() const noexcept { return m_handle; }
    Widget* parent() const noexcept { return m_pParent; }
    void setParent(Widget* p) noexcept { m_pParent = p; }
    Widget* rootParent() noexcept;

    auto padding() const noexcept { return m_paddingFunc(this); }
    void setPadding(const decltype(m_paddingFunc)& func) noexcept { m_paddingFunc = func; }
    template<typename Format>
    void setPadding(Format, Format, Format, Format) noexcept;

    virtual void setColor(const glm::vec4& color) noexcept { m_color = color; }
    virtual const glm::vec4 &color() const noexcept { return m_color; }
    virtual void setForegroundColor(const glm::vec4& color) noexcept { m_foregroundColor = color; }
    virtual const glm::vec4& foregroundColor() const noexcept { return m_foregroundColor; }
    const glm::vec4& borderColor() const noexcept { return m_borderColor; }
    void setBorderColor(const glm::vec4& c) noexcept { m_borderColor = c; }
    auto borderThickness() const noexcept { return m_borderThickness; }
    void setBorderThickness(Pixels f) noexcept { m_borderThickness = f; }
    auto cornerRadius() const noexcept { return m_cornerRadius; }
    void setCornerRadius(Pixels f) noexcept { m_cornerRadius = f; }

    const auto& children() const noexcept { return m_vChildren; }
    auto& children() noexcept { return m_vChildren; }
    virtual void addChild(Widget*, size_t nDrawOrder = 1);
    virtual void removeChild(Widget*, bool bMakeRootLevelWidget = false);
    virtual void onAddedToParent() {}
    virtual bool isLayoutWidget() const noexcept { return false; }

    void setPointConstraint(HorizontalAlignment, std::unique_ptr<PointConstraint>);
    void setPointConstraint(VerticalAlignment, std::unique_ptr<PointConstraint>);
    void setSizeConstraint(OrthogonalAxis, std::unique_ptr<SizeConstraint>);

    PointConstraint* getPointConstraint(OrthogonalAxis);
    SizeConstraint* getSizeConstraint(OrthogonalAxis);

    float width(bool bRefresh = false) const {
        if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_fWidthCached.has_value() /*|| (m_pWidth && m_pWidth->dynamic())*/) {
            m_fWidthCached = m_pWidth ? m_pWidth->getValue() : 0.0f;
        }
        return m_fWidthCached.value();
    }
    float height(bool bRefresh = false) const {
        if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_fHeightCached.has_value() /*|| (m_pHeight && m_pHeight->dynamic())*/) {
            m_fHeightCached = m_pHeight ? m_pHeight->getValue() : 0.0f;
        }
        return m_fHeightCached.value();
    }
    void setCachedWidth(float f) noexcept { m_fWidthCached = f; }
    void setCachedHeight(float f) noexcept { m_fHeightCached = f; }
    float dimension(OrthogonalAxis axis, bool bRefresh = false) const {
        float fRet{ -1.0f };
        switch (axis) {
        case OrthogonalAxis::HORIZONTAL:
            fRet = width(bRefresh);
            break;
        case OrthogonalAxis::VERTICAL:
            fRet = height(bRefresh);
            break;
        default:
            break;
        }
        return fRet;
    }
    float contentWidth() const {
        return right() - left();
    }
    float contentHeight() const {
        return top() - bottom();
    }
    float contentDimension(OrthogonalAxis axis) const {
        float fRet{ -1.0f };
        switch (axis) {
        case OrthogonalAxis::HORIZONTAL:
            fRet = contentWidth();
            break;
        case OrthogonalAxis::VERTICAL:
            fRet = contentHeight();
            break;
        default:
            break;
        }
        return fRet;
    }
    std::tuple<float, float, float, float> extents(bool bRefresh = false) const noexcept;
    std::tuple<float, float, float, float> clipFrame(bool bRefresh = false) const noexcept;
    void setClipFrameFunc(const std::function<std::tuple<float, float, float, float>(const Widget*)>& func) noexcept { m_clipFrameFunc = func; }
    auto hasClipFrameFunc() const noexcept { return m_clipFrameFunc != nullptr; }

    bool isInsideWidget(float, float) const noexcept;
    std::pair<float, float> globalToLocalCoords(float, float) const noexcept;

    float posX(bool bRefresh = false) const {
        if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_fXCached.has_value() /*|| (m_pX.second && m_pX.second->dynamic())*/) {
            m_fXCached = m_pX.second->getValue();
        }
        return m_fXCached.value();
    }
    float posY(bool bRefresh = false) const {
        if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_fYCached.has_value() /*|| (m_pY.second && m_pY.second->dynamic())*/) {
            m_fYCached = m_pY.second->getValue();
        }
        return m_fYCached.value();
    }
    glm::vec2 position() const {
        return glm::vec2{ posX(), posY() };
    }
    HorizontalAlignment horizAlignment() const noexcept { return m_pX.first; }
    VerticalAlignment vertAlignment() const noexcept { return m_pY.first; }

    float top(bool bWithPadding = true) const;
    float bottom(bool bWithPadding = true) const;
    float left(bool bWithPadding = true) const;
    float right(bool bWithPadding = true) const;

    void setOnMouseOverFunc(const decltype(m_mouseOverFunc)& func) noexcept { m_mouseOverFunc = func; }
    void setOnMouseEnterFunc(const decltype(m_mouseEnterFunc)& func) noexcept { m_mouseEnterFunc = func; }
    void setOnMouseLeaveFunc(const decltype(m_mouseLeaveFunc)& func) noexcept { m_mouseLeaveFunc = func; }
    void setOnMouseButtonFunc(const decltype(m_mouseButtonFunc)& func) noexcept { m_mouseButtonFunc = func; }
    void setOnWindowResizedFunc(const decltype(m_windowResizedFunc)& func) noexcept { m_windowResizedFunc = func; }
    void setOnFocusReceivedFunc(const decltype(m_focusReceivedFunc)& func) noexcept { m_focusReceivedFunc = func; }
    void setOnFocusLostFunc(const decltype(m_focusLostFunc)& func) noexcept { m_focusLostFunc = func; }
    void setOnTextInputFunc(const decltype(m_textInputFunc)& func) noexcept { m_textInputFunc = func; }

    void onMouseOver(float x, float y, float offsetX, float offsetY, std::uint8_t buttonMask, bool& bHandled) noexcept;
    void onMouseEnter(bool&) noexcept;
    void onMouseLeave(bool&) noexcept;
    void onMouseButton(MouseButton, bool, bool&) noexcept;
    void onFocusReceived() noexcept;
    void onFocusLost() noexcept;
    void onWindowResized(int, int) noexcept;
    void onTextInput(const sTextInputInfo&) noexcept;

    bool& isMouseOver() noexcept { return m_bIsMouseOver; }
    const bool& isMouseOver() const noexcept { return m_bIsMouseOver; }
    bool hasChildren() const noexcept { return children().size() != 0; }
    
    bool hasFocus() const noexcept { return m_bHasFocus; }
    void setHasFocus(bool b) noexcept { m_bHasFocus = b; }

    // isTabStoppable identifies widgets as being receivers of tab stops, i.e.
    // "main" widgets that appear as a single entity to the user
    // Therefore embedded widgets used to build more complex widgets are not
    // tabStoppable, which is reflected by the fact that createWidget() sets
    // this property to false if called with Contained=true
    bool isTabStoppable() const noexcept { return m_bIsTabStoppable; }
    void setIsTabStoppable(bool b) noexcept { m_bIsTabStoppable = b; }

    bool apply(const std::function<bool(Widget*)>&);
    bool isInHierarchy(Widget*) noexcept;
    
    const auto& animations() const noexcept { return m_vAnimations; }
    auto& animations() noexcept { return m_vAnimations; }
    void addAnimation(AnimationTarget, float, float, bool);

    template<IsMeasurementUnit T = NormalizedPixels, IsMeasurementUnit U = NormalizedPixels>
    void addPresetConstraints(ConstraintPreset p, Widget* pParent, T fWidth = NormalizedPixels{ -1.0f }, U fHeight = NormalizedPixels{ -1.0f });

    virtual void render(IWidgetRenderer* pRenderer);
    virtual void refreshConstraints();
    void clearConstraintsCache();
    
    template<typename PointConstr, typename Alignment, typename... Args>
    void setPointConstraint(Alignment align, Args&&... args) {
        static_assert(std::is_base_of_v<PointConstraint, PointConstr> && (std::is_same_v<Alignment, HorizontalAlignment> || std::is_same_v<Alignment, VerticalAlignment>),
            "Alignment type must either be Horizontal or VerticalAlignment, PointConstr must be derived from PointConstraint");
        setPointConstraint(align, std::make_unique<PointConstr>(std::forward<Args>(args)...));
    }

    template<typename SizeConstr, typename... Args>
    void setSizeConstraint(OrthogonalAxis axis, Args&&... args) {
        static_assert(std::is_base_of_v<SizeConstraint, SizeConstr>,
            "SizeConstr must be derived from SizeConstraint");
        setSizeConstraint(axis, std::make_unique<SizeConstr>(std::forward<Args>(args)...));
    }
};

}

}


#endif /* Widget_h */
