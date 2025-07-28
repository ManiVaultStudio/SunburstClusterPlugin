var isQtAvailable = false;

// pipe errors to log
window.onerror = function (msg, url, num) {
    log("Sunburst: Error: " + msg + "\nURL: " + url + "\nLine: " + num);
};

// auto log for Qt and console
function log(logtext) {

	if (isQtAvailable) {
		QtBridge.js_debug(logtext.toString());
	}
	else {
		console.log(logtext);
	}
}

log("Sunburst: try connecting to qt");

try {
    new QWebChannel(qt.webChannelTransport, function (channel) {
        // Establish connection
        QtBridge = channel.objects.QtBridge;

        // register signals
        QtBridge.qt_setDataInJS.connect(function (json, flag) { 
            const data_json = JSON.parse(json); 
            draw_sunburst_with_new_data(data_json, flag); 
        });
        QtBridge.qt_setOptInJS.connect(function (flag) { 
            draw_sunburst_with_new_opt(flag); 
        });

        // confirm successful connection
        isQtAvailable = true;

        QtBridge.js_available();

        log("Sunburst: successfully connected to qt");
    });
} catch (error) {
    log("Sunburst: failed to connect to qt. " + error);
}

function notifyManiVaultAboutSelectedClusters(selectionJSON)
{
    if (isQtAvailable) {
        const selectionString = JSON.stringify(selectionJSON);
        QtBridge.js_passSelectedClustersToQt(selectionString);
    }
}
