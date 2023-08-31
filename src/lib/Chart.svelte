<script>
  import { Chart, Card } from "flowbite-svelte";

  export let data;

  const options = {
    series: data.map((d) => d.value),
    colors: data.map((d) => d.color),
    chart: {
      height: 180,
      width: 180,
      type: "donut",
    },
    stroke: {
      colors: ["transparent"],
      lineCap: "",
    },
    plotOptions: {
      pie: {
        donut: {
          labels: {
            show: true,
            name: {
              color: "#eee",
              show: true,
              fontFamily: "Inter, sans-serif",
              offsetY: 20,
            },
            total: {
              color: "#eee",
              showAlways: true,
              show: true,
              label: "Total",
              fontFamily: "Inter, sans-serif",
              formatter: function (w) {
                const sum = w.globals.seriesTotals.reduce((a, b) => {
                  return a + b;
                }, 0);
                return `${sum}`;
              },
            },
            value: {
              color: "#eee",
              show: true,
              fontFamily: "Inter, sans-serif",
              offsetY: -20,
            },
          },
          size: "90%",
        },
      },
    },
    grid: {
      padding: {
        top: -2,
      },
    },
    labels: data.map((d) => d.label),
    dataLabels: {
      enabled: data.length <= 3,
      style: {
        colors: ["#eee"],
      },
    },
    legend: {
      labels: {
        colors: data.map(_ => "#eee"),
      },
      position: "bottom",
      fontFamily: "Inter, sans-serif",
    },
    yaxis: {
      labels: {
        style: {
          colors: ["#eee"],
        },
        formatter: function (value) {
          return value;
        },
      },
    },
    xaxis: {
      labels: {
        style: {
          colors: ["#eee"],
        },
        formatter: function (value) {
          return value;
        },
      },
      axisTicks: {
        show: false,
      },
      axisBorder: {
        show: false,
      },
    },
  };
</script>

<Card>
  <Chart {options} />
</Card>
