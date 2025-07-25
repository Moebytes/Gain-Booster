import React, {useState, useRef, useEffect, useContext} from "react"
import {ThemeContext} from "../index"
import withJuceSlider, {WithJUCESliderProps} from "./withJuceSlider"
import withJuceComboBox, {WithJUCEComboBoxProps} from "./withJuceComboBox"
import MiniKnob from "./MiniKnob"
import functions from "../structures/Functions"
import squareWave from "../assets/square-wave.png"
import sineWave from "../assets/sine-wave.png"
import sawWave from "../assets/saw-wave.png"
import triangleWave from "../assets/triangle-wave.png"
import musicNote from "../assets/music-note.png"
import "./styles/lfobar.scss"

interface LFOTypeProps {
    parameterID: string
    label: string
    color: string
}

const LFOBarType: React.FunctionComponent<LFOTypeProps & WithJUCEComboBoxProps> = ({color, label, parameterID, value, onChange}) => {
    const {theme, setTheme} = useContext(ThemeContext)
    
    const getSelectFilter= () => {
        return theme === "light" ? "brightness(0) invert(0)" : "brightness(0) invert(1)"
    }

    const filter = functions.calculateFilter(color)

    const waves = {
        square: <img className="wave-icon" src={squareWave} style={{filter}} draggable={false}/>,
        saw: <img className="wave-icon" src={sawWave} style={{filter}} draggable={false}/>,
        triangle: <img className="wave-icon" src={triangleWave} style={{filter}} draggable={false}/>,
        sine: <img className="wave-icon" src={sineWave} style={{filter}} draggable={false}/>
    }

    const wavesJSX = () => {
        let jsx = [] as React.ReactElement[]

        for (let i = 0; i < Object.entries(waves).length; i++) {
            const [wave, img] = Object.entries(waves)[i]
            jsx.push(
                <div key={`${parameterID}-${i}`}
                    className={`wave-option ${value === i ? "selected" : ""}`}
                    onClick={() => onChange(i)}
                    style={{filter: value === i ? getSelectFilter() : ""}}>
                    {img}
                </div>
            )
        }

        return jsx
    }

    return (
        <>
        <span className="lfobar-label" style={{color}}>{`${label} LFO`}</span>
        <div className="lfobar-waves">{wavesJSX()}</div>
        </>
    )
}

interface LFORateProps {
    parameterID: string
    color: string
}

const LFOBarRate: React.FunctionComponent<LFORateProps & WithJUCESliderProps> = ({color, value, onChange}) => {
    const allowedNumerators = [1, 2, 3, 4]
    const allowedDenominators = [1, 2, 4, 8, 16, 32]
    const initialValues = functions.getFraction(value, allowedNumerators, allowedDenominators)

    const [numerator, setNumerator] = useState(initialValues.numerator)
    const [denominator, setDenominator] = useState(initialValues.denominator)
    const draggingPart = useRef<"num" | "den" | null>(null)
    const dragAccumulator = useRef(0)

    const filter = functions.calculateFilter(color)

    useEffect(() => {
        const handleMouseMove = (e: MouseEvent) => {
            if (!draggingPart.current) return
        
            dragAccumulator.current += e.movementY
            const threshold = 10
        
            if (dragAccumulator.current <= -threshold) {
                if (draggingPart.current === "num") {
                    setNumerator(prev => {
                        const i = allowedNumerators.indexOf(prev)
                        return allowedNumerators[Math.min(allowedNumerators.length - 1, i + 1)]
                    })
                } else if (draggingPart.current === "den") {
                    setDenominator(prev => {
                        const i = allowedDenominators.indexOf(prev)
                        return allowedDenominators[Math.min(allowedDenominators.length - 1, i + 1)]
                    })
                }
                dragAccumulator.current = 0
            }
        
            if (dragAccumulator.current >= threshold) {
                if (draggingPart.current === "num") {
                    setNumerator(prev => {
                        const i = allowedNumerators.indexOf(prev)
                        return allowedNumerators[Math.max(0, i - 1)]
                    })
                } else if (draggingPart.current === "den") {
                    setDenominator(prev => {
                        const i = allowedDenominators.indexOf(prev)
                        return allowedDenominators[Math.max(0, i - 1)]
                    })
                }
                dragAccumulator.current = 0
            }
        }

        const handleMouseUp = () => {
            draggingPart.current = null
            dragAccumulator.current = 0
        }

        window.addEventListener("mousemove", handleMouseMove)
        window.addEventListener("mouseup", handleMouseUp)
        return () => {
            window.removeEventListener("mousemove", handleMouseMove)
            window.removeEventListener("mouseup", handleMouseUp)
        }
    }, [])

    useEffect(() => {
        onChange(numerator / denominator)
    }, [numerator, denominator])

    return (
        <div className="lfobar-rate">
            <span className="lfobar-rate-part" onMouseDown={() => (draggingPart.current = "num")}>
                {numerator}
            </span>
            <span className="lfobar-rate-slash">/</span>
            <span className="lfobar-rate-part" onMouseDown={() => (draggingPart.current = "den")}>
                {denominator}
            </span>
            <img className="lfobar-rate-icon" src={musicNote} style={{filter}} draggable={false}/>
        </div>
    )
}

interface Props {
    lfoTypeID: string
    lfoRateID: string
    lfoAmountID: string
    label: string
    color: string
    style?: React.CSSProperties
}

const LFOBarTypeCombo = withJuceComboBox(LFOBarType)
const LFOBarRateSlider = withJuceSlider(LFOBarRate)

const LFOBar: React.FunctionComponent<Props> = ({lfoTypeID, lfoRateID, lfoAmountID, label, color}) => {
    return (
        <div className="lfobar-container">
            <LFOBarTypeCombo parameterID={lfoTypeID} color={color} label={label}/>
            <LFOBarRateSlider parameterID={lfoRateID} color={color}/>
            <MiniKnob parameterID={lfoAmountID} color={color} label={label}/>
        </div>
    )    
}

export default LFOBar