<script lang="ts">
  import Icon from "@iconify/svelte";
  export let icon: string;
  export let color: string;
  export let iconColor: string;
  export let bgColor = "#282828";
  export let borderMode = "solid";

  let ringClass = "";
  if (borderMode == "solid") {
    ringClass = ``;
  } else if (borderMode == "gradient") {
    // ringClass = `bg-gradient-to-br from-[${color}] to-[${bgColor}]`;
    ringClass =
      "bg-gradient-to-br from-purple-600 to-blue-500 hover:bg-gradient-to-bl focus:ring-blue-300 dark:focus:ring-blue-800";
  }
</script>

<div
  class={`p-0.5 rounded-lg flex-inline ${ringClass}`}
  style:background-color={borderMode == "solid" ? color : "transparent"}
>
  <button
    type="button"
    role="button"
    class="
  flex flex-row gap-1 items-center
    h-7
  text-center font-medium
  focus:ring-4 focus:outline-none
  px-2
  text-sm
  rounded-lg inline-flex justify-center
    w-full
    !border-0 !rounded-md
    hover:bg-transparent hover:!text-inherit
  transition-all duration-75 ease-in
  group-hover:!bg-opacity-0 group-hover:!text-inherit"
    style:border-color={color}
    style:background-color={bgColor}
    style:color
    on:click
  >
    <Icon {icon} color={iconColor || color} />
    <slot />
  </button>
</div>
