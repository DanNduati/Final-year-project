const mc = require("@mindconnect/mindconnect-nodejs");
const os = require("os");

// Asynchronous wrapper for the main function
(async () => {
    const agent = new mc.MindConnectAgent(require("/key/key.json"));
    const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

    try {
            if (!agent.IsOnBoarded()) {
                await agent.OnBoard();
            }

            if (!agent.HasDataSourceConfiguration()) {
                const result = await agent.GetDataSourceConfiguration();
            }

            console.log(await agent.GetDataSourceConfiguration());

            for (let index = 0; index < 10000; index++) {
                const values = [
                    { "dataPointId": "1545600142904", "qualityCode": "0", "value": "69" },   //Temp
                    { "dataPointId": "1545600155998", "qualityCode": "0", "value":  "55" } , // Humidity
                ];
                console.log(values);
                // this is how to send the data with specific timestamp
                // await agent.PostData(values, new Date(Date.now() - 86400 * 1000));
                agent.PostData(values);                
                await sleep(60000);
            }
        }
        catch (err) {
            console.log(err);
            process.exit(1);
        }
    })();