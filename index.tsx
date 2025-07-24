import React from "react"
import {createRoot} from "react-dom/client"
import Knob from "./components/Knob"
import LFOBar from "./components/LFOBar"
import parameters from "./processor/parameters.json"
import "./index.scss"

const App: React.FunctionComponent = (props) => {

    return (
        <div className="app">
            <div className="title-container">
                <span className="title-text">Gain <span className="title-highlight">Booster</span></span>
            </div>
            <div className="knobs-container">
                <Knob label={parameters.gain.id.toUpperCase()} parameterID={parameters.gain.id} mapOptionID={parameters.gainSkew.id}
                color="#ff0db2" display="percent" mappingOptions={["logarithmic", "linear", "exponential"]}/>
                <Knob label={parameters.boost.id.toUpperCase()} parameterID={parameters.boost.id} mapOptionID={parameters.boostSkew.id}
                color="#227eff" display="decibels" style={{marginLeft: "10px"}} mappingOptions={["logarithmic", "linear", "exponential"]}/>
                <Knob label={parameters.pan.id.toUpperCase()} parameterID={parameters.pan.id} mapOptionID={parameters.panningLaw.id}
                color="#460dff" display="pan" mappingOptions={["constant", "triangle", "linear"]}/>
            </div>
            <div className="lfo-container">
                <LFOBar label={parameters.gain.id.toUpperCase()} lfoTypeID={parameters.gainLFOType.id} lfoRateID={parameters.gainLFORate.id} 
                lfoAmountID={parameters.gainLFOAmount.id} color="#ff0db2"/>
                <LFOBar label={parameters.pan.id.toUpperCase()} lfoTypeID={parameters.panLFOType.id} lfoRateID={parameters.panLFORate.id} 
                lfoAmountID={parameters.panLFOAmount.id}  color="#460dff"/>
            </div>
        </div>
    )

}

const root = createRoot(document.getElementById("root")!)
root.render(<App/>)