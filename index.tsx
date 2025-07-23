import React from "react"
import {createRoot} from "react-dom/client"
import JuceSlider from "./components/JuceSlider"
import Knob from "./components/Knob"
import "./index.scss"

const App: React.FunctionComponent = (props) => {

    return (
        <div className="app">
            <Knob label="GAIN" parameterID="gain"/>
        </div>
    )

}

const root = createRoot(document.getElementById("root")!)
root.render(<App/>)