//
//  ModelPreview.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelPreview_h
#define ModelPreview_h

#include <IceEngine.h>
#include <GUI/Widget.h>
#include <glm/mat4x4.hpp>
#include <System/Model.h>

namespace Ice {

class IWidgetRenderer;

namespace GUI {

class ICEENGINE_API ModelPreview : public Widget {
    //Model *m_pModel{ nullptr };
    std::vector<Model> m_vModels;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_modelMatrix;
    
public:
    ModelPreview(const std::vector<Model>& vModels, const glm::mat4& m, const glm::mat4& transf = glm::mat4{ 1.0f }) : m_vModels{ vModels }, m_viewMatrix{ m }, m_modelMatrix{ transf } {
        setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        setForegroundColor(glm::vec4{ -1.0f, -1.0f, -1.0f, -1.0f });
        setGraphicsDirty(true);
    }
    ModelPreview(const Model& model, const glm::mat4& m, const glm::mat4& transf = glm::mat4{ 1.0f }) : ModelPreview{ std::vector<Model>{ model }, m, transf } {}
    
    void render(IWidgetRenderer*) override;
    
    decltype(auto) models() const noexcept { return (m_vModels); }
    void setModel(Model model) noexcept { m_vModels = { model }; setGraphicsDirty(true); }
    void setModels(const std::vector<Model>& vModels) noexcept { m_vModels = vModels; setGraphicsDirty(true); }

    const auto& viewMatrix() const noexcept { return m_viewMatrix; }
    void setViewMatrix(const glm::mat4& m) noexcept { m_viewMatrix = m; setGraphicsDirty(true); }
    
    const auto& modelMatrix() const noexcept { return m_modelMatrix; }
    void setModelMatrix(const glm::mat4& m) noexcept { m_modelMatrix = m; setGraphicsDirty(true); }
};

}

}

#endif /* ModelPreview_h */
