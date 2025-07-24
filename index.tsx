import React from "react"
import {createRoot} from "react-dom/client"
import Knob from "./components/Knob"
import parameters from "./processor/parameters.json"
import "./index.scss"

const App: React.FunctionComponent = (props) => {

    return (
        <div className="app">
            <div className="knobs-container">
                <Knob label={parameters.gain.id.toUpperCase()} parameterID={parameters.gain.id} color="#ff0db2" display="percent"/>
                <Knob label={parameters.boost.id.toUpperCase()} parameterID={parameters.boost.id} color="#227eff" display="decibels"/>
                <Knob label={parameters.pan.id.toUpperCase()} parameterID={parameters.pan.id} color="#460dff" display="pan"/>
            </div>
        </div>
    )

}

const root = createRoot(document.getElementById("root")!)
root.render(<App/>)