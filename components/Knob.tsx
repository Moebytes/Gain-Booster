import React, {useId} from "react"
import {KnobHeadless, KnobHeadlessLabel, KnobHeadlessOutput, useKnobKeyboardControls} from "react-knob-headless"
import JuceSlider from "./JuceSlider"
import MappingOptions from "./MappingOptions"
import functions from "../structures/Functions"
import "./styles/knob.scss"

interface Props {
    parameterID: string
    mapOptionID: string
    label: string
    color: string
    display: "percent" | "decibels" | "pan"
    roundFunction?: (value: number) => number
    displayFunction?: (value: number) => string
    style?: React.CSSProperties
    mappingOptions?: ("logarithmic" | "linear" | "exponential" | "triangle" | "constant")[]
}

const Knob: React.FunctionComponent<Props> = ({label, parameterID, mapOptionID, color, display, roundFunction, displayFunction, style, mappingOptions}) => {
    const knobID = useId()
    const labelID = useId()

    return (
        <JuceSlider parameterID={parameterID}>
            {({value, properties, onChange, reset, dragStart, dragEnd}) => {
                const minAngle = -145
                const maxAngle = 145
                const min = properties.start
                const max = properties.end
                const step = (max - min) / (properties.numSteps - 1)
                const angle = functions.remapRange(value, min, max, minAngle, maxAngle)

                if (!roundFunction) roundFunction = (value: number) => value

                if (!displayFunction) displayFunction = (value: number) => `${(value * 100).toFixed(0)}%`

                if (display === "decibels") {
                    displayFunction = (value: number) => `${(value * 12).toFixed(1)} dB`
                }

                if (display === "pan") {
                    displayFunction = (value: number) => `${Math.round((value - 0.5) * 100)} ${(value - 0.5) < 0 ? "L" : "R"}`;
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
                    <div className="knob-container" style={{...style}}>
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
                            onDragStart={dragStart}
                            onDragEnd={dragEnd}
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
                        <MappingOptions mapOptionID={mapOptionID} color={color} mappingOptions={mappingOptions}/>
                    </div>
                )
            }}
        </JuceSlider>
    )
}

export default Knob