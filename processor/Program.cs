using DataGen;
using static DataGen.Formula;

public class Processor
{
    record PairedFormula(Dictionary<string, PerfCalcDelegate> Base, Dictionary<string, PerfCalcDelegate> Huge);

    static readonly PairedFormula[] FormulaLib = new PairedFormula[3]
    {
        new(Native4K, Native2M),
        new(Virt4K, Virt2M),
        new(Nested4K, Nested2M),
    };

    static bool TryRead(string path, out string[] lines)
    {
        try
        {
            lines = File.ReadAllLines(path);
            return true;
        }
        catch
        {
            Console.WriteLine($"ERROR: Cannot open file {path} for read.");

            lines = Array.Empty<string>();
            return false;
        }
    }

    private static readonly string[] PrintOrder = new[]
        { "Vanilla", "FPT", "ECPT", "ASAP", "Agile Paging", "DMT", "pvDMT" };

    static (decimal lat, decimal ovh, decimal time) CalculatePerf(string[] perf)
    {
        var perfData = perf
            .Select(x => x.Split("\t", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
            .Where(x => x.Length == 2)
            .ToDictionary(x => x[0], x => decimal.Parse(x[1]));

        var walkOverhead =
            (perfData["dtlb_load_misses.walk_duration"] + perfData["dtlb_store_misses.walk_duration"] + perfData["itlb_misses.walk_duration"] + perfData["ept.walk_cycles"]) * 2 / 
            (perfData["cycles:ukhHG"] * 2); // Because each core has two walkers

        var walkLatency =
            (perfData["dtlb_load_misses.walk_duration"] + perfData["dtlb_store_misses.walk_duration"] + perfData["itlb_misses.walk_duration"] + perfData["ept.walk_cycles"]) * 2 / 
            (perfData["dtlb_load_misses.walk_completed"] + perfData["dtlb_store_misses.walk_completed"] + perfData["itlb_misses.walk_completed"]);

        var time = perfData["duration"];

        return (walkLatency, walkOverhead, time);
    }

    static void PrintResult(decimal overhead, Dictionary<string, decimal> lat, decimal sptOverhead)
    {
        var baseline = lat["Vanilla"];

        foreach (var key in PrintOrder)
        {
            if (!lat.ContainsKey(key))
                continue;

            var value = lat[key];
            var pwSpeedup = baseline / value;

            Console.WriteLine($"Page Walk Speedup of {key}: {pwSpeedup:0.00}x");
        }

        Console.WriteLine();

        foreach (var key in PrintOrder)
        {
            if (!lat.ContainsKey(key))
                continue;

            var value = lat[key];
            var runtime = (1 - overhead) + overhead * value / baseline;
            var rtSpeedup = 1 / runtime;

            // DMT does not use SPT for nested virtualization
            if (key == "pvDMT")
            {
                rtSpeedup *= sptOverhead;
            }

            Console.WriteLine($"Application Speedup of {key}: {rtSpeedup:0.00}x");
        }

        Console.WriteLine();
    }

    static void ParseResult(PairedFormula formula, string[] basePerf, string[] hugePerf, string[] sim, decimal sptOverhead, bool allowNative)
    {
        //
        // Perf Data
        //

        var baseStat = CalculatePerf(basePerf);
        var hugeStat = CalculatePerf(hugePerf);

        Console.WriteLine($"Workload Runtime: Base {baseStat.time:0.00} sec / Huge {hugeStat.time:0.00} sec");
        Console.WriteLine($"Page Walk Overhead: Base {baseStat.ovh * 100:0.0}% / Huge {hugeStat.ovh * 100:0.0}%");
        Console.WriteLine($"Average Walk Latency: Base {baseStat.lat:0.0} cycles / Huge {hugeStat.lat:0.0} cycles");
        Console.WriteLine();

        //
        // Simulator Data
        //

        var simData = new RawData(sim);

        if (simData.nativeFormat && !allowNative) {
            Console.WriteLine($"ERROR: Cannot calculate results for virtualized case when using data generated with native simulator.");
            Environment.Exit(1);
            return;
        }

        var baseResult = simData.PullAverage(formula.Base);
        var hugeResult = simData.PullAverage(formula.Huge);

        Console.WriteLine("Base Page Result ----------------");

        PrintResult(baseStat.ovh, baseResult, sptOverhead);

        Console.WriteLine("Huge Page Result ----------------");

        PrintResult(hugeStat.ovh, hugeResult, sptOverhead);
    }

    public static int Main(string[] args)
    {
        //
        // Parse Parameter
        //

        if (args.Length < 3)
        {
            Console.WriteLine($"dotnet run -- [base_folder] [workload_name] [native/virt/nested] [[ept_runtime_for_nested_only]] [[spt_runtime_for_nested_only]]");
            Environment.Exit(1);
            return 1;
        }

        var baseFolder = args[0];
        var workload = args[1];
        var level = args[2];
        var ept = args.Length >= 5 ? args[3] : "invalid";
        var spt = args.Length >= 5 ? args[4] : "invalid";

        PairedFormula formula;

        var allowNative = false;

        switch (level.ToUpper())
        {
        case "NATIVE":
            formula = FormulaLib[0];
            allowNative = true;
            break;
        case "VIRT":
            formula = FormulaLib[1];
            break;
        case "NESTED":
            formula = FormulaLib[2];
            break;
        default:
            Console.WriteLine($"ERROR: Invalid virtualization level value {level}, must be one of: native, virt, nested.");
            Environment.Exit(1);
            return 1;
        }

        var spovh = 1.0m;

        if (level.ToUpper() == "NESTED")
        {
            if (!decimal.TryParse(spt, out var sptt))
            {
                Console.WriteLine($"ERROR: Invalid SPT runtime value {spt}, must be a number.");
                Environment.Exit(1);
                return 1;
            }

            if (!decimal.TryParse(ept, out var eptt))
            {
                Console.WriteLine($"ERROR: Invalid EPT runtime value {ept}, must be a number.");
                Environment.Exit(1);
                return 1;
            }

            spovh = sptt / eptt;
        }

        var baseFile = Path.Combine(baseFolder, $"perf-{workload}", "perf-never.short");
        var hugeFile = Path.Combine(baseFolder, $"perf-{workload}", "perf-always.short");
        var simFile = Path.Combine(baseFolder, $"trace-{workload}", "sim.log");

        if (!TryRead(baseFile, out var baseLines) || !TryRead(hugeFile, out var hugeLines) || !TryRead(simFile, out var simLines))
        {
            Environment.Exit(1);
            return 1;
        }

        ParseResult(formula, baseLines, hugeLines, simLines, spovh, allowNative);

        return 0;
    }
}
