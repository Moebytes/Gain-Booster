import React, {useState} from "react"
import JuceComboBox from "./JuceComboBox"
import "./styles/Mappingoptions.scss"

interface Props {
    mapOptionID: string
    color: string
    mappingOptions?: ("logarithmic" | "linear" | "exponential" | "triangle" | "constant")[]
}

const MappingOptions: React.FunctionComponent<Props> = ({mapOptionID, color, mappingOptions = []}) => {
    return (
        <JuceComboBox parameterID={mapOptionID}>
            {({value, properties, onChange, reset}) => {

                const shapes = {
                    logarithmic: <path d="M2,18 C6,4 10,2 18,2" stroke="currentColor" fill="none" strokeWidth="2"/>,
                    linear: <line x1="2" y1="18" x2="18" y2="2" stroke="currentColor" strokeWidth="2"/>,
                    exponential: <path d="M2,18 C10,18 14,10 18,2" stroke="currentColor" fill="none" strokeWidth="2"/>,
                    triangle: <polygon points="10,2 18,18 2,18" fill="currentColor"/>,
                    constant: <circle cx="10" cy="10" r="8" fill="currentColor"/>
                }
            
                const generateJSX = () => {
                    let jsx = [] as React.ReactElement[]
            
                    for (let i = 0; i < mappingOptions.length; i++) {
                        const option = mappingOptions[i]
                        jsx.push(
                            <div key={`${mapOptionID}-${i}`}
                                className={`mapping-option ${value === i ? "selected" : ""}`}
                                onClick={() => onChange(i)}
                                style={{color: value === i ? "white" : color}}>
                                <svg viewBox="0 0 20 20" width="15" height="15">
                                    {shapes[option]}
                                </svg>
                            </div>
                        )
                    }
                    return jsx
                }
            
                return (
                    <div className="mapping-options" style={{borderColor: color}}>
                        {generateJSX()}
                    </div>
                )
            }}
        </JuceComboBox>
    )
}

export default MappingOptions