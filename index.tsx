import React, {useState, useEffect} from "react"
import {createRoot} from "react-dom/client"
import Knob from "./components/Knob"
import LFOBar from "./components/LFOBar"
import parameters from "./processor/parameters.json"
import dark from "./assets/dark.png"
import light from "./assets/light.png"
import functions from "./structures/Functions"
import "./index.scss"

const darkColorList = {
    "--background": "#0d0d0d",
    "--text": "white",
    "--border": "black"
}

const lightColorList = {
    "--background": "white",
    "--text": "black",
    "--border": "white"
}

type ThemeContextType = {theme: string; setTheme: React.Dispatch<React.SetStateAction<string>>}
export const ThemeContext = React.createContext<ThemeContextType>({theme: "", setTheme: () => null})

const App: React.FunctionComponent = () => {
    const [theme, setTheme] = useState(localStorage.getItem("theme") || "dark")

    useEffect(() => {
        const colorList = theme === "light" ? lightColorList : darkColorList
        for (const [key, color] of Object.entries(colorList)) {
            document.documentElement.style.setProperty(key, color)
        }
        localStorage.setItem("theme", theme)
    }, [theme])

    const toggleTheme = () => {
        setTheme((prev) => prev === "light" ? "dark" : "light")
    }

    const filter = functions.calculateFilter("#ff0db2")

    return (
        <div className="app">
            <ThemeContext.Provider value={{theme, setTheme}}>
            <div className="title-container">
                <span className="title-text">Gain <span className="title-highlight">Booster</span></span>
                <img className="theme-icon" src={theme === "light" ? dark : light} style={{filter}}onClick={toggleTheme} draggable={false}/>
            </div>
            <div className="knobs-container">
                <Knob 
                    label={parameters.gain.id.toUpperCase()} 
                    parameterID={parameters.gain.id} 
                    mapOptionID={parameters.gainCurve.id}
                    color="#ff0db2" 
                    display="percent" 
                    mappingOptions={["logarithmic", "linear", "exponential"]}/>
                <Knob 
                    label={parameters.boost.id.toUpperCase()} 
                    parameterID={parameters.boost.id} 
                    mapOptionID={parameters.boostCurve.id}
                    color="#227eff" 
                    display="decibels" 
                    style={{marginLeft: "10px"}} 
                    mappingOptions={["logarithmic", "linear", "exponential"]}/>
                <Knob 
                    label={parameters.pan.id.toUpperCase()} 
                    parameterID={parameters.pan.id} 
                    mapOptionID={parameters.panningLaw.id}
                    color="#460dff" 
                    display="pan" 
                    mappingOptions={["constant", "triangle", "linear"]}/>
            </div>
            <div className="lfo-container">
                <LFOBar 
                    label={parameters.gain.id.toUpperCase()}
                    lfoTypeID={parameters.gainLFOType.id} 
                    lfoRateID={parameters.gainLFORate.id} 
                    lfoAmountID={parameters.gainLFOAmount.id} 
                    color="#ff0db2"/>
                <LFOBar 
                    label={parameters.pan.id.toUpperCase()} 
                    lfoTypeID={parameters.panLFOType.id} 
                    lfoRateID={parameters.panLFORate.id} 
                    lfoAmountID={parameters.panLFOAmount.id}  
                    color="#460dff"/>
            </div>
            </ThemeContext.Provider>
        </div>
    )

}

const root = createRoot(document.getElementById("root")!)
root.render(<App/>)