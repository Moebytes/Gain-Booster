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
}