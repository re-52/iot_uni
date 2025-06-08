const express = require('express');
const axios = require('axios');

const app = express();
const PORT = 3000;

app.use(express.json());

app.get('/data', async (req, res) => {
    try {
        const response = await axios.get('https://callback-iot.onrender.com/data');
        const data = response.data;

        const lastTwo = data.slice(-2);
        res.json(lastTwo);
    } catch (error) {
        console.error('Hubo un Error:', error.message);
        res.status(500).json({ error: 'Error al obtener los datos' });
    }
});

app.post('/data', async (req, res) => {
    const data = req.body;

    try {
        await axios.post('http://localhost:1880/data', data);
        res.status(200).json({ message: 'Datos enviados correctamente a Node-RED' });
    } catch (error) {
        console.error('Hubo un Error:', error.message);
        res.status(500).json({ error: 'Los datos no pudieron ser enviados' });
    }
});

app.get('/data/hex', async (req, res) => {
    try {
        const response = await axios.get('https://callback-iot.onrender.com/data');
        const data = response.data;

        const hexItem = data.find(item => item.hexData);

        if (!hexItem) {
            return res.status(404).json({ error: 'No hay datos encriptados' });
        }

        const hexString = hexItem.hexData;
        const buffer = Buffer.from(hexString, 'hex');

        if (buffer.length < 12) {
            return res.status(400).json({ error: 'hexData no tiene suficientes bytes' });
        }

        const temp = buffer.readFloatLE(0);
        const hum = buffer.readFloatLE(4);
        const pressure = buffer.readFloatLE(8);

        const device = hexItem.device || 'Sin dispositivo';
        const timestamp = hexItem.timestamp || 'Sin fecha';

        const fechaLocal = new Date(timestamp).toLocaleString('es-ES', {
            timeZone: 'UTC',
            hour12: false
        });

        const respuestaFormateada = `
        Dispositivo: ${device}
        Fecha: ${fechaLocal}
        Temperatura: ${temp.toFixed(2)} C
        Humedad: ${hum.toFixed(1)} %
        Presion: ${pressure.toFixed(1)} hPa`;

        res.setHeader('Content-Type', 'text/plain');
        res.send(respuestaFormateada);
    } catch (error) {
        console.error('Hubo un Error al procesar hexData:', error.message);
        res.status(500).json({ error: 'Error al procesar hexData' });
    }
});

app.listen(PORT, () => {
    console.log(`Servidor corriendo en http://localhost:${PORT}`);
});
