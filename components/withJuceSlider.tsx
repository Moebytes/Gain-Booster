import React, {useEffect, useState} from "react"
import * as JUCE from "juce-framework-frontend-mirror"
import functions from "../structures/Functions"

interface JUCESliderProperties {
    start: number
    end: number
    skew: number
    name: string
    label: string
    numSteps: number
    interval: number
    parameterIndex: number
}

export interface WithJUCESliderProps {
    value: number
    properties: JUCESliderProperties
    onChange: (value: number) => void
    reset: () => void
    dragStart: () => void
    dragEnd: () => void
}

interface WithParameter {
    parameterID: string
}

const getDefaultParameter = JUCE.getNativeFunction("getDefaultParameter")

const withJuceSlider = <Props extends object & WithParameter>(
    WrappedComponent: React.ComponentType<Props & WithJUCESliderProps>, 
): React.FunctionComponent<Props> => {

    const JuceSlider: React.FunctionComponent<Props> = (props) => {
        const {parameterID} = props
        const sliderState = JUCE.getSliderState(parameterID)!
        const [properties, setProperties] = useState(sliderState.properties)
        const [value, setValue] = useState(sliderState.getScaledValue())
        const [dragging, setDragging] = useState(false)

        useEffect(() => {
            setValue(sliderState.getScaledValue())
            setProperties(sliderState.properties)
        }, [])
    
        useEffect(() => {
            const valueID = sliderState.valueChangedEvent.addListener(() => {
                setValue(sliderState.getScaledValue())
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
            const normalized = functions.normalizeRange(value, properties.start, properties.end)
            sliderState.setNormalisedValue(normalized)
            setValue(value)
        }
    
        const handleReset = async () => {
            const defaultValue = await getDefaultParameter(parameterID)
            handleChange(defaultValue)
        }
    
        const handleDragStart = () => {
            sliderState.sliderDragStarted()
            setDragging(true)
        }
    
        const handleDragEnd = () => {
            sliderState.sliderDragEnded()
            setDragging(false)
        }
    
        useEffect(() => {
            const handleMouseUp = () => {
                if (dragging) handleDragEnd()
            }
            document.addEventListener("mouseup", handleMouseUp)
            return () => {
                document.removeEventListener("mouseup", handleMouseUp)
            }
        }, [dragging])
    
        return (
            <WrappedComponent
                {...props}
                value={value}
                properties={properties}
                onChange={handleChange}
                reset={handleReset}
                dragStart={handleDragStart}
                dragEnd={handleDragEnd}
            />
        )
    }

    return JuceSlider
}

export default withJuceSlider