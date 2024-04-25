import React, { useState } from 'react';
import { StyleSheet, View, Button, Text, ScrollView } from 'react-native';
import { LineChart } from 'react-native-chart-kit';
import { Dimensions } from 'react-native';

const screenWidth = Dimensions.get('window').width;

const App = () => {
    const [voltammogramData, setVoltammogramData] = useState({
        labels: [],
        datasets: []
    });

    const handlePress = async () => {
        try {
            const response = await fetch('http://172.20.10.3/startPulses', {
                method: 'GET'
            });
            const json = await response.json();

            // Define a new line color for the new dataset
            const lineColor = `rgba(${Math.floor(Math.random() * 255)}, ${Math.floor(Math.random() * 255)}, ${Math.floor(Math.random() * 255)}, 1)`;

            const potentialData = json.data.map(item => item.potential.toFixed(2));
            const currentData = json.data.map(item => item.differentialCurrent >= 0 ? item.differentialCurrent : 0);

            setVoltammogramData(prevData => ({
                labels: potentialData,
                datasets: [
                    ...prevData.datasets,
                    {
                        data: currentData,
                        color: () => lineColor, // Use the line color for the dataset
                        strokeWidth: 2,
                        propsForDots: {
                            r: '6',
                            strokeWidth: '2',
                            stroke: lineColor // Match the dot color with the line color
                        }
                    }
                ]
            }));
        } catch (error) {
            console.error('Error fetching data:', error);
        }
    };

    const handleClearGraph = () => {
        setVoltammogramData({
            labels: [],
            datasets: []
        });
    };

    return (
        <ScrollView style={styles.scrollViewStyle}>
            <View style={styles.container}>
                <Text style={styles.heading}>Differential Pulse Voltammogram</Text>
                <View style={styles.buttonContainer}>
                    <Button title="Generate Pulses" onPress={handlePress} />
                    <Button title="Clear Graph" onPress={handleClearGraph} color="red" />
                </View>
                {voltammogramData.datasets.length > 0 && (
                    <LineChart
                        data={voltammogramData}
                        width={screenWidth - 16}
                        height={220}
                        chartConfig={{
                            backgroundColor: '#ffffff',
                            backgroundGradientFrom: '#fb8c00',
                            backgroundGradientTo: '#ffa726',
                            decimalPlaces: 6,
                            color: (opacity = 1) => `rgba(0, 0, 0, ${opacity})`,
                            labelColor: (opacity = 1) => `rgba(0, 0, 0, ${opacity})`,
                            style: {
                                borderRadius: 16
                            }
                        }}
                        bezier
                        style={{
                            marginVertical: 8,
                            borderRadius: 16
                        }}
                    />
                )}
            </View>
        </ScrollView>
    );
};

const styles = StyleSheet.create({
    scrollViewStyle: {
        backgroundColor: '#F5FCFF'
    },
    container: {
        flex: 1,
        alignItems: 'center',
        justifyContent: 'center',
        padding: 20
    },
    heading: {
        fontSize: 18,
        fontWeight: 'bold',
        margin: 50
    },
    buttonContainer: {
        flexDirection: 'row',
        justifyContent: 'space-around',
        alignItems: 'center',
        width: '100%',
        marginBottom: 20,
    }
});
export default App;
