var data = [];                 // JSON data
var opt_zoom = false;

function getContainerDimensions() {
    const chartContainer = document.getElementById('chart');
    const containerRect = chartContainer.getBoundingClientRect();

    // Use container dimensions, fallback to reasonable defaults
    const width = containerRect.width > 0 ? containerRect.width : 400;
    const height = containerRect.height > 0 ? containerRect.height : 400;

    return { width, height };
}

function draw_sunburst() {
    log("Sunburst: draw_sunburst");

    const { width, height } = getContainerDimensions();

    const chart = opt_zoom ? sunburst_zoom(data, width, height) : sunburst_static(data, width, height);

    d3.select('#chart')
        .selectAll('*')
        .remove();

    d3.select('#chart')
        .node()
        .appendChild(chart);
}

function draw_sunburst_with_new_data(d_in, opt_zoom_in) {
    log("Sunburst: draw_sunburst_with_new_data");

    data = d_in
    opt_zoom = opt_zoom_in

    draw_sunburst()
}

function draw_sunburst_with_new_opt(opt_zoom_in) {
    log("Sunburst: draw_sunburst_with_new_opt");

    opt_zoom = opt_zoom_in

    draw_sunburst()
}

//// Select single item (clears others)
//chart.selectPartition("Category A");

//// Add to selection (preserves others)
//chart.selectPartition("Category B", true);

//// Get current selection
//const selected = chart.getSelection();
//console.log("Currently selected:", selected);

//// Clear all selections
//chart.clearSelection();

// Simple resize handler
window.onresize = function () {
    if (data && data.length > 0) {
        draw_sunburst();
    }
};
