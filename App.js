import React from 'react';
import { StyleSheet, View, Button } from 'react-native';

const App = () => {
    const handlePress = async () => {
        try {
            const response = await fetch('http://192.168.43.101/startPulses', {
                method: 'GET'
            });
            const json = await response.json();
            console.log(json);
            // Use json.status and json.data to update the UI
        } catch (error) {
            console.error('Error fetching data:', error);
        }
    };
    return (
        <View style={styles.container}>
            <Button title="Generate Pulses" onPress={handlePress} />
        </View>
    );
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        backgroundColor: '#F5FCFF',
    },
});

export default App;
