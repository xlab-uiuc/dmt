namespace DataGen
{
    public enum Item
    {
        G1H1, G1H2, G1H3, G1H4, G1,
        G2H1, G2H2, G2H3, G2H4, G2,
        G3H1, G3H2, G3H3, G3H4, G3,
        G4H1, G4H2, G4H3, G4H4, G4,
        UDH1, UDH2, UDH3, UDH4, FREQ,
    }

    public record RawLine(decimal[] raw)
    {
        public decimal[] raw = raw;

        public decimal G1H1 => raw[(int)Item.G1H1];
        public decimal G1H2 => raw[(int)Item.G1H2];
        public decimal G1H3 => raw[(int)Item.G1H3];
        public decimal G1H4 => raw[(int)Item.G1H4];
        public decimal G1 => raw[(int)Item.G1];

        public decimal G2H1 => raw[(int)Item.G2H1];
        public decimal G2H2 => raw[(int)Item.G2H2];
        public decimal G2H3 => raw[(int)Item.G2H3];
        public decimal G2H4 => raw[(int)Item.G2H4];
        public decimal G2 => raw[(int)Item.G2];

        public decimal G3H1 => raw[(int)Item.G3H1];
        public decimal G3H2 => raw[(int)Item.G3H2];
        public decimal G3H3 => raw[(int)Item.G3H3];
        public decimal G3H4 => raw[(int)Item.G3H4];
        public decimal G3 => raw[(int)Item.G3];

        public decimal G4H1 => raw[(int)Item.G4H1];
        public decimal G4H2 => raw[(int)Item.G4H2];
        public decimal G4H3 => raw[(int)Item.G4H3];
        public decimal G4H4 => raw[(int)Item.G4H4];
        public decimal G4 => raw[(int)Item.G4];

        public decimal UDH1 => raw[(int)Item.UDH1];
        public decimal UDH2 => raw[(int)Item.UDH2];
        public decimal UDH3 => raw[(int)Item.UDH3];
        public decimal UDH4 => raw[(int)Item.UDH4];

        public decimal FREQ => raw[(int)Item.FREQ];
    }

    public record RawData
    {
        public List<RawLine> rawData;
        public bool nativeFormat;

        public RawData(string[] text)
        {
            rawData = new List<RawLine>();
            nativeFormat = false;

            // Find the final report segment
            var lastPart = text.Length - 1;
            for (; lastPart >= 0; lastPart--)
            {
                if (text[lastPart].StartsWith("~~~~~~~~~"))
                    break;
            }

            // Parse data lines
            for(var i = lastPart + 1; i < text.Length; i++)
            {
                var line = text[i];
                var temp = LineBreakdown(line);

                if (temp == null)
                    continue;

                rawData.Add(temp);
            }
        }

        public Dictionary<string, decimal> PullAverage(Dictionary<string, Formula.PerfCalcDelegate> group)
        {
            var result = new Dictionary<string, decimal>();

            foreach(var kvp in group)
            {
                var count = 0m;
                var total = 0m;

                foreach (var point in rawData)
                {
                    count += point.FREQ;
                    total += kvp.Value(point) * point.FREQ;
                }

                result[kvp.Key] = total / count;
            }

            return result;
        }

        private RawLine? LineBreakdown(string line)
        {
            if (string.IsNullOrWhiteSpace(line))
                return null;

            var break1 = line.Split('\t', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
            
            if (break1.Length != 2)
                return null;

            var break2 = break1[0].Split(',', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);

            // Handle native data format
            if (break2.Length == 5)
            {
                nativeFormat = true;

                var tmp = new string[25];

                for (var i = 0; i < tmp.Length; i++)
                    tmp[i] = "ZERO";

                // Expand into 24 step format
                tmp[(int)Item.G1] = break2[0];
                tmp[(int)Item.G2] = break2[1];
                tmp[(int)Item.G3] = break2[2];
                tmp[(int)Item.G4] = break2[3];

                break2 = tmp;
            }

            if (break2.Length != 25)
                return null;

            // Overwrite the useless RANGE_HIT with frequency
            break2[(int)Item.FREQ] = break1[1];

            var data = new decimal[25];

            // Translate values
            for(var i = 0; i < data.Length; i++)
            {
                var temp = TranslateValue(break2[i]);

                if (temp == null)
                    return null;

                data[i] = temp.Value;
            }

            return new RawLine(data);
        }

        private decimal? TranslateValue(string val)
        {
            switch(val)
            {
                case "ZERO": return 0;
                case "PWC": return Constants.PWC;
                case "L1": return Constants.L1;
                case "L2": return Constants.L2;
                case "LLC": return Constants.L3;
                case "MEMORY": return Constants.MEM;

                default:
                    return decimal.TryParse(val, out var num) ? num : null;
            }
        }
    }
}
