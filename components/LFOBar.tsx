import React, {useState, useRef, useEffect} from "react"
import MiniKnob from "./MiniKnob"
import functions from "../structures/Functions"
import squareWave from "../assets/square-wave.png"
import sineWave from "../assets/sine-wave.png"
import sawWave from "../assets/saw-wave.png"
import triangleWave from "../assets/triangle-wave.png"
import musicNote from "../assets/music-note.png"
import "./styles/lfobar.scss"

interface Props {
    parameterID: string
    label: string
    color: string
    style?: React.CSSProperties
}

const LFOBar: React.FunctionComponent<Props> = ({parameterID, label, color}) => {
    const [selected, setSelected] = useState("")
    const [numerator, setNumerator] = useState(1)
    const [denominator, setDenominator] = useState(8)
    const draggingPart = useRef<"num" | "den" | null>(null)
    const dragAccumulator = useRef(0)

    const filter = functions.calculateFilter(color)

    const allowedNumerators = [1, 2, 3, 4]
    const allowedDenominators = [1, 2, 4, 8, 16, 32]

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

    const waves = {
        square: <img className="wave-icon" src={squareWave} style={{filter}} draggable={false}/>,
        saw: <img className="wave-icon" src={sawWave} style={{filter}} draggable={false}/>,
        triangle: <img className="wave-icon" src={triangleWave} style={{filter}} draggable={false}/>,
        sine: <img className="wave-icon" src={sineWave} style={{filter}} draggable={false}/>
    }

    const wavesJSX = () => {
        let jsx = [] as React.ReactElement[]

        for (const [wave, img] of Object.entries(waves)) {
            jsx.push(
                <div key={`${parameterID}-${wave}`}
                    className={`wave-option ${selected === wave ? "selected" : ""}`}
                    onClick={() => setSelected(wave)}
                    style={{filter: selected === wave ? "brightness(0) invert(1)" : ""}}>
                    {img}
                </div>
            )
        }

        return jsx
    }
    return (
        <div className="lfobar-container">
            <span className="lfobar-label" style={{color}}>{`${label} LFO`}</span>
            <div className="lfobar-waves">{wavesJSX()}</div>

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

            <MiniKnob parameterID={parameterID} color={color} label={label}/>
        </div>
    )
}

export default LFOBar