import React, {useState} from "react"
import * as JUCE from "juce-framework-frontend-mirror"
import "./styles/presetbar.scss"

const openPresetMenu = JUCE.getNativeFunction("openPresetMenu")

const PresetBar: React.FunctionComponent = () => {
    const prevPreset = () => {

    }

    const nextPreset = () => {

    }

    const presetMenu = async () => {
        await openPresetMenu()
    }

    const shapes = {
        leftArrow: <polygon points="15,6 9,12 15,18" fill="currentColor"/>,
        rightArrow: <polygon points="9,6 15,12 9,18" fill="currentColor"/>
    }

    return (
        <div className="preset-bar">
            <span className="preset-title">Preset:</span>
            <span className="preset-name" onClick={presetMenu}>Default</span>
            <div className="preset-arrows">
                <svg className="preset-left-arrow" onClick={prevPreset} xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                    {shapes.leftArrow}
                </svg>
                <svg className="preset-right-arrow" onClick={nextPreset} xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                    {shapes.rightArrow}
                </svg>
            </div>
        </div>
    )
}

export default PresetBar