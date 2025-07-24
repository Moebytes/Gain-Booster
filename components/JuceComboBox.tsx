import React, {useEffect, useState} from "react"
import * as JUCE from "juce-framework-frontend-mirror"

interface JUCEComboProperties {
    name: string
    parameterIndex: number
    choices: string[]
}

interface Props {
    parameterID: string
    children: (props: {
        value: number
        properties: JUCEComboProperties
        onChange: (value: number) => void
        reset: () => void
    }) => React.ReactNode
}

const getDefaultParameter = JUCE.getNativeFunction("getDefaultParameter")

const JuceComboBox: React.FunctionComponent<Props> = ({parameterID, children}) => {
    const comboState = JUCE.getComboBoxState(parameterID)!
    const [properties, setProperties] = useState(comboState.properties)
    const [value, setValue] = useState(comboState.getChoiceIndex())

    useEffect(() => {
        const valueID = comboState.valueChangedEvent.addListener(() => {
            setValue(comboState.getChoiceIndex())
        })
        const propsID = comboState.propertiesChangedEvent.addListener(() => {
            setProperties(comboState.properties)
        })
        return () => {
            comboState.valueChangedEvent.removeListener(valueID)
            comboState.propertiesChangedEvent.removeListener(propsID)
        }
    }, [])

    const handleChange = (index: number) => {
        comboState.setChoiceIndex(index)
        setValue(index)
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
                reset: handleReset
            })}
        </>
    )
}

export default JuceComboBox