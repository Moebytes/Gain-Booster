import React, {useState, useEffect} from "react"
import * as JUCE from "juce-framework-frontend-mirror"
import "./styles/presetbar.scss"

const openPresetMenu = JUCE.getNativeFunction("openPresetMenu")
const prevPreset = JUCE.getNativeFunction("prevPreset")
const nextPreset = JUCE.getNativeFunction("nextPreset")


const PresetBar: React.FunctionComponent = () => {
    const [preset, setPreset] = useState("Default")

    useEffect(() => {
        window.__JUCE__.backend.addEventListener("presetChanged", changePreset)
    }, [])

    const prev = async () => {
        const prev = await prevPreset()
        if (prev) setPreset(prev)
    }

    const next = async () => {
        const next = await nextPreset()
        if (next) setPreset(next)
    }

    const presetMenu = async () => {
        const name = await openPresetMenu()
        if (name) setPreset(name)
    }

    const changePreset = (name: string) => {
        setPreset(name)
    }

    const shapes = {
        leftArrow: <polygon points="15,6 9,12 15,18" fill="currentColor"/>,
        rightArrow: <polygon points="9,6 15,12 9,18" fill="currentColor"/>
    }

    return (
        <div className="preset-bar">
            <span className="preset-title">Preset:</span>
            <span className="preset-name" onClick={presetMenu}>{preset}</span>
            <div className="preset-arrows">
                <svg className="preset-left-arrow" onClick={prev} xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                    {shapes.leftArrow}
                </svg>
                <svg className="preset-right-arrow" onClick={next} xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                    {shapes.rightArrow}
                </svg>
            </div>
        </div>
    )
}

export default PresetBar