import React, {useEffect, useState} from "react"
import {createRoot} from "react-dom/client"
import * as JUCE from "juce-framework-frontend-mirror"
import "./index.scss"

const gainState = JUCE.getSliderState("gain")!

const App: React.FunctionComponent = (props) => {
    const [gain, setGain] = useState(gainState.getNormalisedValue())
    const [gainProperties, setGainProperties] = useState(gainState.properties)

    useEffect(() => {
        const valueID = gainState.valueChangedEvent.addListener(() => {
            setGain(gainState.getScaledValue())
        })
        const propertyID = gainState.propertiesChangedEvent.addListener(() => {
            setGainProperties(gainState.properties)
        })
        return () => {
            gainState.valueChangedEvent.removeListener(valueID)
            gainState.propertiesChangedEvent.removeListener(propertyID)
        }
    }, [])

    const changeGain = (value: number) => {
        gainState.setNormalisedValue(value)
        setGain(value)
    }

    const dragStart = () => {
        gainState.sliderDragStarted()
    }

    const dragEnd = () => {
        gainState.sliderDragEnded()
    }
 
    return <input type="range" id="gainKnob" step={1 / (gainProperties.numSteps - 1)} defaultValue={gain} 
    onMouseDown={dragStart} onMouseUp={dragEnd} min={gainProperties.start} max={gainProperties.end} 
    onChange={(event) => changeGain(Number(event.target.value))}></input>
}

const root = createRoot(document.getElementById("root")!)
root.render(<App/>)