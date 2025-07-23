import React, {useEffect, useState} from "react"
import * as JUCE from "juce-framework-frontend-mirror"

interface JUCEProperties {
    start: number
    end: number
    skew: number
    name: string
    label: string
    numSteps: number
    interval: number
    parameterIndex: number
}

interface Props {
    parameterID: string
    children: (props: {
        value: number
        properties: JUCEProperties
        onChange: (value: number) => void
        reset: () => void
        dragStart: () => void
        dragEnd: () => void
    }) => React.ReactNode
}

const getDefaultParameter = JUCE.getNativeFunction("getDefaultParameter")

const JuceSlider: React.FunctionComponent<Props> = ({parameterID, children}) => {
    const sliderState = JUCE.getSliderState(parameterID)!
    const [value, setValue] = useState(sliderState.getNormalisedValue())
    const [properties, setProperties] = useState(sliderState.properties)

    useEffect(() => {
        const valueID = sliderState.valueChangedEvent.addListener(() => {
            setValue(sliderState.getNormalisedValue())
        })
        const propsID = sliderState.propertiesChangedEvent.addListener(() => {
            setProperties(sliderState.properties)
        })
        return () => {
            sliderState.valueChangedEvent.removeListener(valueID)
            sliderState.propertiesChangedEvent.removeListener(propsID)
        }
    }, [])

    const handleChange = (value: number) => {
        sliderState.setNormalisedValue(value)
        setValue(value)
    }

    const handleReset = async () => {
        const defaultValue = await getDefaultParameter(parameterID)
        handleChange(defaultValue)
    }

    return (
        <>
            {children({
                value,
                properties,
                onChange: handleChange,
                reset: handleReset,
                dragStart: sliderState.sliderDragStarted,
                dragEnd: sliderState.sliderDragEnded
            })}
        </>
    )
}

export default JuceSlider