import {Color, Solver, hexToRgb} from "./Color"

export default class Functions {
    
    public static normalizeRange = (value: number, min: number, max: number) => {
        return (value - min) / (max - min)
    }

    public static denormalizeValue = (value: number, min: number, max: number) => {
        return value * (max - min) + min
    }

    public static remapRange = (value: number, min: number, max: number, newMin: number, newMax: number) => {
        const normalized = Functions.normalizeRange(value, min, max)
        return Functions.denormalizeValue(normalized, newMin, newMax)
    }

    public static calculateFilter = (hexColor: string) => {
        const rgb = hexToRgb(hexColor) as any
        if (!rgb) return ""
        const color = new Color(rgb[0], rgb[1], rgb[2])
        const solver = new Solver(color)
        const result = solver.solve()
        return result.filter
    }

    public static getFraction = (target: number, numerators: number[], denominators: number[]) => {
        let closestNum = 1
        let closestDen = 1
        let minDiff = Infinity
    
        for (const num of numerators) {
            for (const den of denominators) {
                const val = num / den
                const diff = Math.abs(val - target)
                if (diff < minDiff) {
                    minDiff = diff
                    closestNum = num
                    closestDen = den
                }
            }
        }
    
        return {numerator: closestNum, denominator: closestDen}
    }
}