var notification = {

_queue: [],

_delay: 3000,

_k_limit: 3,

init:
function init() {
    notification.check_proc();
    setInterval(notification.check_proc, 1000);
},

check_proc:
function check_proc() {
    if (notification._queue.length) {
        var tuple = notification._queue.shift();
        notification.notify(tuple[0], tuple[1], tuple[2], tuple[3]);
    }
},

notify:
function notify(title, summary, image, type) {
    title = title.replace(/&gt;/g, '>').replace(/&lt;/g, '<').replace(/&amp;/g, '&');
    summary = summary.replace(/&gt;/g, '>').replace(/&lt;/g, '<').replace(/&amp;/g, '&');
    if (util.is_native_platform()) {
        twuner_action('system/notify/'
            + type
            + '/' + encodeURIComponent(title)
            + '/' + encodeURIComponent(summary)
            + '/' + encodeURIComponent(image));
    } else if (conf.vars.platform == 'Chrome') {
        var img_url = image? image: './image/ic64_twuner.png';
        var notification = webkitNotifications.createNotification(img_url, title, summary);
        notification.show();
        setTimeout(function() {notification.cancel()}, 5000);
    }
},

push:
function push(title, summary, image, type) {
    notification._queue.push([title, summary, image, type]);
}

};
