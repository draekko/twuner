if (typeof ext == 'undefined') var ext = {};
ext.Sample = {

id: 'org.twuner.sample',

name: 'Sample',

description: 'Only A Sample Extention.',

version: '1.0',

author: 'Shellex Wai',

url: 'http://draekko.org',

on_add_tweets:
function on_add_tweets(tweets, view) {
    // just for debug.
    twuner_log('Test',
        'Update ['+view.name+'], '+ tweets.length +' items');
},

enable:
function enable() {
    ext.register_listener(ext.ADD_TWEETS_LISTENER_AFTER
        , ext.Sample.on_add_tweets);
},

disable:
function disable() {
    ext.unregister_listener(ext.ADD_TWEETS_LISTENER_AFTER
        , ext.Sample.on_add_tweets);
}

}

