import React, {useId} from "react"
import {KnobHeadless, KnobHeadlessLabel, KnobHeadlessOutput, useKnobKeyboardControls} from "react-knob-headless"
import JuceSlider from "./JuceSlider"
import functions from "../structures/Functions"
import "./styles/knob.scss"

interface Props {
    parameterID: string
    label: string
    color: string
    display: "percent" | "decibels" | "pan"
    roundFunction?: (value: number) => number
    displayFunction?: (value: number) => string
}

const Knob: React.FunctionComponent<Props> = ({label, parameterID, color, display, roundFunction}) => {
    const knobID = useId()
    const labelID = useId()

    return (
        <JuceSlider parameterID={parameterID}>
            {({value, properties, onChange, reset, dragStart, dragEnd}) => {
                const minAngle = -145
                const maxAngle = 145
                const min = 0
                const max = 1
                const step = (max - min) / (properties.numSteps - 1)
                const angle = functions.remapRange(value, min, max, minAngle, maxAngle)

                if (!roundFunction) roundFunction = (value: number) => value

                let displayFunction = (value: number) => {
                    const naturalValue = functions.remapRange(value, min, max, properties.start, properties.end)
                    return `${(naturalValue * 100).toFixed(0)}%`
                }

                if (display === "decibels") {
                    displayFunction = (value: number) => {
                        const naturalValue = functions.remapRange(value, min, max, properties.start, properties.end)
                        return `${(naturalValue).toFixed(1)} dB`
                    }
                }

                if (display === "pan") {
                    displayFunction = (value: number) => {
                        const naturalValue = functions.remapRange(value, min, max, properties.start, properties.end)
                        return `${+(naturalValue * 50).toFixed(0)} ${naturalValue < 0 ? "L" : "R"}`
                    }
                }

                const keyboardHandler = useKnobKeyboardControls({
                    valueRaw: value,
                    valueMin: min,
                    valueMax: max,
                    step: step,
                    stepLarger: step * 10,
                    onValueRawChange: onChange,
                })

                return (
                    <div className="knob-container">
                        <KnobHeadlessLabel className="knob-label" id={labelID} style={{color}}>
                            {label}
                        </KnobHeadlessLabel>
                        <KnobHeadless
                            aria-label={parameterID}
                            aria-labelledby={labelID}
                            dragSensitivity={0.006}
                            onValueRawChange={onChange}
                            onDoubleClick={reset}
                            valueMin={min}
                            valueMax={max}
                            valueRaw={value}
                            valueRawDisplayFn={displayFunction}
                            valueRawRoundFn={roundFunction}
                            onMouseDown={dragStart}
                            onMouseUp={dragEnd}
                            axis="y"
                            style={{outline: "none"}}
                            {...keyboardHandler}>
                            <div className="knob" style={{backgroundColor: color}}>
                                <div className="knob-rotator" style={{transform: `rotate(${angle}deg)`}}>
                                    <div className="knob-indicator"/>
                                </div>
                            </div>
                        </KnobHeadless>
                        <KnobHeadlessOutput className="knob-value" htmlFor={knobID}>
                            {displayFunction(value)}
                        </KnobHeadlessOutput>
                    </div>
                )
            }}
        </JuceSlider>
    )
}

export default Knob