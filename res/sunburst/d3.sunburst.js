// Function to determine if a color is dark and needs white text
function isColorDark(hexColor) {
    // Convert hex to RGB
    const hex = hexColor.replace('#', '');
    const r = parseInt(hex.substr(0, 2), 16);
    const g = parseInt(hex.substr(2, 2), 16);
    const b = parseInt(hex.substr(4, 2), 16);
    
    // Calculate luminance using the relative luminance formula
    const luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255;
    
    // Return true if dark (luminance < 0.5 means dark)
    return luminance < 0.5;
}

// https://observablehq.com/@d3/zoomable-sunburst
// ISC, Mike Bostock
function sunburst_zoom(data, containerWidth, containerHeight)
{
  // Calculate radius based on container size, with some padding
  const padding = 10;
  const availableWidth = containerWidth - padding * 2;
  const availableHeight = containerHeight - padding * 2;
  const radius = Math.min(availableWidth, availableHeight) / 6;

  // Scale font size with radius
  const fontSize = Math.max(8, Math.min(12, radius / 40)); 

  // We define the color based on the input data
  //const color = d3.scaleOrdinal(d3.quantize(d3.interpolateRainbow, data.children.length + 1));

  // Compute the layout.
  const hierarchy = d3.hierarchy(data)
      .sum(d => d.value)
      .sort((a, b) => b.value - a.value);
  const root = d3.partition()
      .size([2 * Math.PI, hierarchy.height + 1])
    (hierarchy);
  root.each(d => d.current = d);

  // Create the arc generator.
  const arc = d3.arc()
      .startAngle(d => d.x0)
      .endAngle(d => d.x1)
      .padAngle(d => Math.min((d.x1 - d.x0) / 2, 0.005))
      .padRadius(radius * 1.5)
      .innerRadius(d => d.y0 * radius)
      .outerRadius(d => Math.max(d.y0 * radius, d.y1 * radius - 1))

  // Create the SVG container.
  const svg = d3.create("svg")
      //.attr("viewBox", [-width / 2, -height / 2, width, width])
      .attr("viewBox", [-radius * 3, -radius * 3, radius * 6, radius * 6])
      .style("font", `${fontSize}px sans-serif`)
      .style("background", "white");

  // Append the arcs.
  const path = svg.append("g")
      .selectAll("path")
      .data(root.descendants().slice(1))
      .join("path")
      .attr("fill", d => d.data.color)
      .attr("fill-opacity", d => arcVisible(d.current) ? 1 : 0)
      .attr("pointer-events", d => arcVisible(d.current) ? "auto" : "none")
      .attr("d", d => arc(d.current));

  // Make them clickable if they have children.
  path.filter(d => d.children)
      .style("cursor", "pointer")
      .on("click", clicked);

  const format = d3.format(",d");
  path.append("title")
      .text(d => `${d.ancestors().map(d => d.data.name).reverse().join("/")}\n${format(d.value)}`);

  const label = svg.append("g")
      .attr("pointer-events", "none")
      .attr("text-anchor", "middle")
      .style("user-select", "none")
      .selectAll("text")
      .data(root.descendants().slice(1))
      .join("text")
      .attr("dy", "0.35em")
      .attr("fill-opacity", d => +labelVisible(d.current))
      .attr("transform", d => labelTransform(d.current))
      .attr("fill", d => { return isColorDark(d.data.color) ? "white" : "black"; })
      .text(d => d.data.name);

  const parent = svg.append("circle")
      .datum(root)
      .attr("r", radius)
      .attr("fill", "none")
      .attr("pointer-events", "all")
      .on("click", clicked);

  // Handle zoom on click.
  function clicked(event, p) {
    parent.datum(p.parent || root);

    root.each(d => d.target = {
      x0: Math.max(0, Math.min(1, (d.x0 - p.x0) / (p.x1 - p.x0))) * 2 * Math.PI,
      x1: Math.max(0, Math.min(1, (d.x1 - p.x0) / (p.x1 - p.x0))) * 2 * Math.PI,
      y0: Math.max(0, d.y0 - p.depth),
      y1: Math.max(0, d.y1 - p.depth)
    });

    const t = svg.transition().duration(event.altKey ? 7500 : 750);

    // Transition the data on all arcs, even the ones that aren’t visible,
    // so that if this transition is interrupted, entering arcs will start
    // the next transition from the desired position.
    path.transition(t)
        .tween("data", d => {
          const i = d3.interpolate(d.current, d.target);
          return t => d.current = i(t);
        })
      .filter(function(d) {
        return +this.getAttribute("fill-opacity") || arcVisible(d.target);
      })
        .attr("fill-opacity", d => arcVisible(d.target) ? 1 : 0)
        .attr("pointer-events", d => arcVisible(d.target) ? "auto" : "none") 

        .attrTween("d", d => () => arc(d.current));

    label.filter(function(d) {
        return +this.getAttribute("fill-opacity") || labelVisible(d.target);
      }).transition(t)
        .attr("fill-opacity", d => +labelVisible(d.target))
        .attrTween("transform", d => () => labelTransform(d.current));
  }
  
  function arcVisible(d) {
    return d.y1 <= 3 && d.y0 >= 1 && d.x1 > d.x0;
  }

  function labelVisible(d) {
    return d.y1 <= 3 && d.y0 >= 1 && (d.y1 - d.y0) * (d.x1 - d.x0) > 0.03;
  }

  function labelTransform(d) {
    const x = (d.x0 + d.x1) / 2 * 180 / Math.PI;
    const y = (d.y0 + d.y1) / 2 * radius;
    return `rotate(${x - 90}) translate(${y},0) rotate(${x < 180 ? 0 : 180})`;
  }

  return svg.node();
}

// https://observablehq.com/@d3/sunburst/2
// ISC
function sunburst_static(data, containerWidth, containerHeight) {
    // We define the color based on the input data
    //const color = d3.scaleOrdinal(d3.quantize(d3.interpolateRainbow, data.children.length + 1));

    // Calculate radius based on container size, with some padding
    const padding = 10;
    const availableWidth = containerWidth - padding * 2;
    const availableHeight = containerHeight - padding * 2;
    const radius = Math.min(availableWidth, availableHeight) / 2;

    // Scale font size with radius
    const fontSize = Math.max(8, Math.min(12, radius / 40)); 

    // Track selected element
    let selectedElements = new Set();

    // Prepare the layout.
    const partition = data => d3.partition()
        .size([2 * Math.PI, radius])
        (d3.hierarchy(data)
            .sum(d => d.value)
            .sort((a, b) => b.value - a.value));

    const arc = d3.arc()
        .startAngle(d => d.x0)
        .endAngle(d => d.x1)
        .padAngle(d => Math.min((d.x1 - d.x0) / 2, 0.005))
        .padRadius(radius / 2)
        .innerRadius(d => d.y0)
        .outerRadius(d => d.y1 - 1);

    const root = partition(data);

    // Create the SVG container
    const svg = d3.create("svg")
        .attr("width", radius * 2)
        .attr("height", radius * 2)
        .attr("viewBox", [-radius, -radius, radius * 2, radius * 2])
        .style("max-width", "100%")
        .style("height", "auto")
        .style("background", "white");

    // Add an arc for each element, with a title for tooltips.
    const format = d3.format(",d");
    const pathGroup = svg.append("g");

    const paths = pathGroup
        .selectAll("path")
        .data(root.descendants().filter(d => d.depth))
        .join("path")
        .attr("fill", d => d.data.color)
        .attr("stroke", "white")
        .attr("stroke-width", 1)
        .attr("d", arc)
        .style("cursor", "pointer")
        .on("click", selectOnClick)
        .on("mouseover", function (event, d) {
            // Only add hover effect if not selected
            if (!selectedElements.has(d)) {
                d3.select(this)
                    .style("filter", "brightness(1.05)")
                    .attr("stroke-width", 2);
            }
        })
        .on("mouseout", function (event, d) {
            // Only remove hover effect if not selected
            if (!selectedElements.has(d)) {
                d3.select(this)
                    .style("filter", null)
                    .attr("stroke-width", 1);
            }
        });

    // Add tooltips
    paths.append("title")
        .text(d => `${d.ancestors().map(d => d.data.name).reverse().join("/")}\n${format(d.value)}`);

    // Add a label for each element.
    svg.append("g")
        .attr("pointer-events", "none")
        .attr("text-anchor", "middle")
        .attr("font-size", fontSize)
        .attr("font-family", "sans-serif")
        .selectAll("text")
        .data(root.descendants().filter(d => d.depth && (d.y0 + d.y1) / 2 * (d.x1 - d.x0) > 10))
        .join("text")
        .attr("transform", function(d) {
            const x = (d.x0 + d.x1) / 2 * 180 / Math.PI;
            const y = (d.y0 + d.y1) / 2;
            return `rotate(${x - 90}) translate(${y},0) rotate(${x < 180 ? 0 : 180})`;
        })
        .attr("dy", "0.35em")
        .attr("fill", d => { return isColorDark(d.data.color) ? "white" : "black"; })
        .text(d => d.data.name);

    // Add method to programmatically select/deselect
    function selectOnClick(event, d) {
        const isShiftClick = event.shiftKey;
        const isAlreadySelected = selectedElements.has(d);

        if (isShiftClick) {
            // Shift+click: toggle this element in selection
            if (isAlreadySelected) {
                // Remove from selection
                selectedElements.delete(d);
                d3.select(this)
                    .attr("stroke", "white")
                    .attr("stroke-width", 1)
                    .style("filter", null);
            } else {
                // Add to selection
                selectedElements.add(d);
                d3.select(this)
                    .attr("stroke", "#333")
                    .attr("stroke-width", 3)
                    .style("filter", "brightness(1.1)");
            }
        } else {
            // Normal click
            if (isAlreadySelected && selectedElements.size === 1) {
                // If this is the only selected element, deselect it
                selectedElements.clear();
                d3.select(this)
                    .attr("stroke", "white")
                    .attr("stroke-width", 1)
                    .style("filter", null);
            } else {
                // Clear all selections and select this one
                selectedElements.clear();
                paths.attr("stroke", "white")
                    .attr("stroke-width", 1)
                    .style("filter", null);

                selectedElements.add(d);
                d3.select(this)
                    .attr("stroke", "#333")
                    .attr("stroke-width", 3)
                    .style("filter", "brightness(1.1)");
            }
        }

        // Trigger callbacks
        if (selectedElements.size === 0) {
            if (window.onSunburstDeselect) {
                window.onSunburstDeselect();
            }
        } else {
            if (window.onSunburstSelect) {
                const selectedData = Array.from(selectedElements).map(d => d.data);
                window.onSunburstSelect(selectedData, Array.from(selectedElements));
            }
        }

        const selectedClusterPaths = Array.from(selectedElements).map(d =>
            d.ancestors().map(d => d.data.name).reverse()
        );

        notifyManiVaultAboutSelectedClusters(selectedClusterPaths);
    }

    svg.node().selectPartition = function (dataName) {
        const targetPath = paths.filter(d => d.data.name === dataName);
        if (!targetPath.empty()) {
            targetPath.dispatch('click');
        }
    };

    svg.node().clearSelection = function () {
        if (selectedElement) {
            paths.filter(d => d === selectedElement).dispatch('click');
        }
    };

    return svg.node();
}
