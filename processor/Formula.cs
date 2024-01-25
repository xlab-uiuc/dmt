namespace DataGen
{
    public record Constants
    {
        public static decimal PWC = 1;
        public static decimal L1 = 4;
        public static decimal L2 = 14;
        public static decimal L3 = 54;
        public static decimal MEM = 200;

        // from original papers
        public static decimal ECPT_Native = 2; // CWC + Hash
        public static decimal ECPT_Virt = 4 + 4 + 4; // See NECPT fig 6
        public static decimal ASAP_G3Est = 0; // assume the best
        public static decimal ASAP_G4Est = 0;
    }


    public static class Formula
    {
        public delegate decimal PerfCalcDelegate(RawLine data);

        public static decimal ASAP_Prefetcher(decimal g1, decimal g2, decimal g3)
        {
            var doneG2 = g1 + g2;
            var doneG3 = Math.Max(doneG2 + Constants.ASAP_G3Est, g3);
            return doneG3;
        }

        public static decimal ASAP_Prefetcher(decimal g1, decimal g2, decimal g3, decimal g4)
        {
            var doneG2 = g1 + g2;
            var doneG3 = Math.Max(doneG2 + Constants.ASAP_G3Est, g3);
            var doneG4 = Math.Max(doneG3 + Constants.ASAP_G4Est, g4);
            return doneG4;
        }

        #region Native 4K

        public static Dictionary<string, PerfCalcDelegate> Native4K = new()
        {
            ["Vanilla"] = NativeVanilla4K,
            ["ECPT"] = NativeECPT4K,
            ["FPT"] = NativeFPT4K,
            ["ASAP"] = NativeASAP4K,
            ["DMT"] = NativeHyperlane4K,
        };

        public static decimal NativeVanilla4K(RawLine data)
        {
            return data.G1 + data.G2 + data.G3 + data.G4;
        }

        public static decimal NativeECPT4K(RawLine data)
        {
            return data.G4 + Constants.ECPT_Native;
        }

        public static decimal NativeFPT4K(RawLine data)
        {
            return data.G2 + data.G4;
        }

        public static decimal NativeHyperlane4K(RawLine data)
        {
            return data.G4;
        }

        public static decimal NativeASAP4K(RawLine data)
        {
            return ASAP_Prefetcher(data.G1, data.G2, data.G3, data.G4);
        }

        #endregion

        #region Native 2M

        public static Dictionary<string, PerfCalcDelegate> Native2M = new()
        {
            ["Vanilla"] = NativeVanilla2M,
            ["ECPT"] = NativeECPT2M,
            ["FPT"] = NativeFPT2M,
            ["ASAP"] = NativeASAP2M,
            ["DMT"] = NativeHyperlane2M,
        };

        public static decimal NativeVanilla2M(RawLine data)
        {
            return data.G1 + data.G2 + data.G3;
        }

        public static decimal NativeECPT2M(RawLine data)
        {
            return data.G3 + Constants.ECPT_Native;
        }

        public static decimal NativeFPT2M(RawLine data)
        {
            return data.G1 + data.G3;
        }

        public static decimal NativeHyperlane2M(RawLine data)
        {
            return data.G3;
        }

        public static decimal NativeASAP2M(RawLine data)
        {
            return ASAP_Prefetcher(data.G1, data.G2, data.G3);
        }

        #endregion

        #region Virt 4K

        public static Dictionary<string, PerfCalcDelegate> Virt4K = new()
        {
            ["Vanilla"] = VirtVanilla4K,
            ["ECPT"] = VirtECPT4K,
            ["FPT"] = VirtFPT4K,
            ["ASAP"] = VirtASAP4K,
            ["Agile Paging"] = VirtAgile4K,
            ["DMT"] = VirtHyperlane4K,
            ["pvDMT"] = VirtHyperlanePlus4K,
        };

        public static decimal VirtVanilla4K(RawLine data)
        {
            return
                data.G1H1 + data.G1H2 + data.G1H3 + data.G1H4 + data.G1 +
                data.G2H1 + data.G2H2 + data.G2H3 + data.G2H4 + data.G2 +
                data.G3H1 + data.G3H2 + data.G3H3 + data.G3H4 + data.G3 +
                data.G4H1 + data.G4H2 + data.G4H3 + data.G4H4 + data.G4 +
                data.UDH1 + data.UDH2 + data.UDH3 + data.UDH4;
        }

        public static decimal VirtECPT4K(RawLine data)
        {
            return data.G4H4 + data.G4 + data.UDH4 + Constants.ECPT_Virt;
        }

        public static decimal VirtFPT4K(RawLine data)
        {
            return
                data.G2H2 + data.G2H4 + data.G2 +
                data.G4H2 + data.G4H4 + data.G4 +
                data.UDH2 + data.UDH4;
        }

        public static decimal VirtHyperlane4K(RawLine data)
        {
            return data.G4H4 + data.G4 + data.UDH4;
        }

        public static decimal VirtHyperlanePlus4K(RawLine data)
        {
            return data.G4 + data.UDH4;
        }

        public static decimal VirtASAP4K(RawLine data)
        {
            var actualG1 = ASAP_Prefetcher(data.G1H1, data.G1H2, data.G1H3, data.G1H4) + data.G1;
            var actualG2 = ASAP_Prefetcher(data.G2H1, data.G2H2, data.G2H3, data.G2H4) + data.G2;
            var actualG3 = ASAP_Prefetcher(data.G3H1, data.G3H2, data.G3H3, data.G3H4) + data.G3;
            var actualG4 = ASAP_Prefetcher(data.G4H1, data.G4H2, data.G4H3, data.G4H4) + data.G4;

            return
                ASAP_Prefetcher(actualG1, actualG2, actualG3, actualG4) +
                ASAP_Prefetcher(data.UDH1, data.UDH2, data.UDH3, data.UDH4);
        }

        public static decimal VirtAgile4K(RawLine data)
        {
            return 
                data.G1 + data.G2 + data.G3 + data.G4 +
                data.UDH1 + data.UDH2 + data.UDH3 + data.UDH4;
        }

        #endregion

        #region Virt 2M

        public static Dictionary<string, PerfCalcDelegate> Virt2M = new()
        {
            ["Vanilla"] = VirtVanilla2M,
            ["ECPT"] = VirtECPT2M,
            ["FPT"] = VirtFPT2M,
            ["ASAP"] = VirtASAP2M,
            ["Agile Paging"] = VirtAgile2M,
            ["DMT"] = VirtHyperlane2M,
            ["pvDMT"] = VirtHyperlanePlus2M,
        };

        public static decimal VirtVanilla2M(RawLine data)
        {
            return
                data.G1H1 + data.G1H2 + data.G1H3 + data.G1 +
                data.G2H1 + data.G2H2 + data.G2H3 + data.G2 +
                data.G3H1 + data.G3H2 + data.G3H3 + data.G3 +
                data.UDH1 + data.UDH2 + data.UDH3;
        }

        public static decimal VirtECPT2M(RawLine data)
        {
            return data.G3H3 + data.G3 + data.UDH3 + Constants.ECPT_Virt;
        }

        public static decimal VirtFPT2M(RawLine data)
        {
            return
                data.G1H1 + data.G1H3 + data.G1 +
                data.G3H1 + data.G3H3 + data.G3 +
                data.UDH1 + data.UDH3;
        }

        public static decimal VirtHyperlane2M(RawLine data)
        {
            return data.G3H3 + data.G3 + data.UDH3;
        }

        public static decimal VirtHyperlanePlus2M(RawLine data)
        {
            return data.G3 + data.UDH3;
        }

        public static decimal VirtASAP2M(RawLine data)
        {
            var actualG1 = ASAP_Prefetcher(data.G1H1, data.G1H2, data.G1H3) + data.G1;
            var actualG2 = ASAP_Prefetcher(data.G2H1, data.G2H2, data.G2H3) + data.G2;
            var actualG3 = ASAP_Prefetcher(data.G3H1, data.G3H2, data.G3H3) + data.G3;

            return
                ASAP_Prefetcher(actualG1, actualG2, actualG3) +
                ASAP_Prefetcher(data.UDH1, data.UDH2, data.UDH3);
        }

        public static decimal VirtAgile2M(RawLine data)
        {
            return
                data.G1 + data.G2 + data.G3 +
                data.UDH1 + data.UDH2 + data.UDH3;
        }

        #endregion

        #region Nested

        public static Dictionary<string, PerfCalcDelegate> Nested4K = new()
        {
            ["Vanilla"] = VirtVanilla4K,
            ["pvDMT"] = NestedHyperlane4K,
        };

        public static Dictionary<string, PerfCalcDelegate> Nested2M = new()
        {
            ["Vanilla"] = VirtVanilla2M,
            ["pvDMT"] = NestedHyperlane2M,
        };

        public static decimal NestedHyperlane4K(RawLine data)
        {
            // Emulate three dimension page walks
            return (data.G4 + data.UDH4) / 2 * 3;
        }

        public static decimal NestedHyperlane2M(RawLine data)
        {
            // Emulate three dimension page walks
            return (data.G3 + data.UDH3) / 2 * 3;
        }

        #endregion
    }
}
