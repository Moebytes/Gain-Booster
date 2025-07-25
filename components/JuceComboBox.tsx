import React, {useEffect, useState} from "react"
import * as JUCE from "juce-framework-frontend-mirror"

interface JUCEComboProperties {
    name: string
    parameterIndex: number
    choices: string[]
}

export interface WithJUCEComboBoxProps {
    value: number
    properties: JUCEComboProperties
    onChange: (value: number) => void
    reset: () => void
}

interface WithParameter {
    parameterID: string
}

const getDefaultParameter = JUCE.getNativeFunction("getDefaultParameter")

const withJuceComboBox = <Props extends object & WithParameter>(
    WrappedComponent: React.ComponentType<Props & WithJUCEComboBoxProps>, 
): React.FunctionComponent<Props> => {

        const JuceComboBox: React.FunctionComponent<Props> = (props) => {
            const {parameterID} = props
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
                <WrappedComponent
                    {...props}
                    value={value}
                    properties={properties}
                    onChange={handleChange}
                    reset={handleReset}
                />
            )
        }

        return JuceComboBox
}

export default withJuceComboBox