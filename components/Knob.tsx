import React, {useId} from "react"
import {KnobHeadless, KnobHeadlessLabel, KnobHeadlessOutput, useKnobKeyboardControls} from "react-knob-headless"
import JuceSlider from "./JuceSlider"
import functions from "../structures/Functions"
import "./styles/knob.scss"

interface Props {
    parameterID: string
    label: string
    roundFunction?: (value: number) => number
    displayFunction?: (value: number) => string
}

const Knob: React.FunctionComponent<Props> = ({label, parameterID, roundFunction, displayFunction}) => {
    const knobID = useId()
    const labelID = useId()

    if (!roundFunction) roundFunction = (value: number) => value
    if (!displayFunction) displayFunction = (value: number) => `${(value * 100).toFixed(0)}%`

    return (
        <JuceSlider parameterID={parameterID}>
            {({value, properties, onChange, dragStart, dragEnd}) => {
                const minAngle = -145
                const maxAngle = 145
                const min = properties.start
                const max = properties.end
                const step = (max - min) / (properties.numSteps - 1)
                const angle = functions.remapRange(value, min, max, minAngle, maxAngle)

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
                        <KnobHeadlessLabel className="knob-label" id={labelID}>{label}</KnobHeadlessLabel>
                        <KnobHeadless
                            aria-label={parameterID}
                            aria-labelledby={labelID}
                            dragSensitivity={0.006}
                            onValueRawChange={onChange}
                            valueMin={min}
                            valueMax={max}
                            valueRaw={value}
                            valueRawDisplayFn={displayFunction}
                            valueRawRoundFn={roundFunction}
                            onMouseDown={dragStart}
                            onMouseUp={dragEnd}
                            includeIntoTabOrder={true}
                            axis="y"
                            style={{outline: "none"}}
                            {...keyboardHandler}>
                            <div className="knob">
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