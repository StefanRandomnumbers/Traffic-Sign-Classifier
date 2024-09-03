#include "NeuralNetwork.h"
#include <qdebug.h>

NeuralNetwork::NeuralNetwork()
{

}
NeuralNetwork::NeuralNetwork(QVector<int> layerInfo, double learnFactor) :
    m_layerInfo(layerInfo), m_nHiddenLayers(static_cast<int>(layerInfo.size() - 2)), m_nLayers(static_cast<int>(layerInfo.size())), m_learnFactor(learnFactor)
{
    // initialise the layers
    for (int i = 0; i < layerInfo.size(); i++)
    {
        Layer newLayer(layerInfo[i]);

        m_layers.push_back(newLayer);
    }

    // initialise the weights and bias
    for (int i = 0; i < m_nLayers - 1; i++)
    {
        // weights
        Weight newWeight(layerInfo[i + 1], layerInfo[i]);
        m_weights.push_back(newWeight);

        // init weight dervi
        Weight newWeightDeriv(layerInfo[i + 1], layerInfo[i], 0);
        m_weightsDerivVelocity.push_back(newWeightDeriv);

        // bias
        Layer newBias(layerInfo[i + 1]);
        m_bias.push_back(newBias);

        // init bias deriv
        Layer newBiasDeriv(layerInfo[i + 1], 0);
        m_biasDerivVelocity.push_back(newBiasDeriv);
    }
}

NeuralNetwork::~NeuralNetwork()
{
}

Layer NeuralNetwork::feedforward(Layer input)
{
    m_layers[0] = input;

    for (int layer = 0; layer < m_nLayers - 1; layer++)
    {
        // calculate the activation for the next layer
        m_layers[layer + 1] = m_weights[layer] * m_layers[layer] + m_bias[layer];

        if (layer < m_nLayers - 2) // output layer will not be sigmoidted
        {
            m_layers[layer + 1].sigmoid();
        }
    }

    return m_layers.last();
}

// QPair: first = input; second = label
// returns: trainerror, testerror
QPair<double, double> NeuralNetwork::train(QPair<QList<Layer>, QList<Layer>> trainData, QPair<QList<Layer>, QList<Layer>> testData)
{
    double trainCost = 0;
    double testCost = 0;

    // loop through the training data
    for (int i = 0; i < trainData.first.size(); i++)
    {
        Layer out = feedforward(trainData.first[i]);
        trainCost += (out - trainData.second[i]).pow(2).sum();

        Layer costDeriv = (out - trainData.second[i]) * 2;  // 2*(x - y) = deriv((x - y)²)
        backpropagation(costDeriv);
    }
    for (int i = 0; i < testData.first.size(); i++)
    {
        Layer testout = feedforward(testData.first[i]);
        testCost += (testout - testData.second[i]).pow(2).sum();
    }

    testCost /= testData.first.size();
    trainCost /= trainData.first.size();
    
    return QPair<double, double>(trainCost, testCost);
}

double NeuralNetwork::train(QPair<Layer, Layer> trainData, QPair<Layer, Layer> testData)
{
    Layer costDeriv = (feedforward(trainData.first) - trainData.second) * 2;  // 2*(x - y)
    backpropagation(costDeriv);

    //double cost = (m_layers.last() - trainData.second).pow(2).sum();
    double testCost = (feedforward(trainData.first) - testData.second).pow(2).sum();
    return testCost;
}

void NeuralNetwork::backpropagation(Layer costDeriv)
{
    Layer h_costDeriv = costDeriv;
    
    // loop backwards through all layers of the nn
    for (int i = m_nLayers - 1; i > 0; i--)
    {
        Layer h_biasDeriv;

        // calculate bias deriv
        if (i < m_nLayers - 1)   // the outputlayer is not activated with the activation function so the deriv of it will be calculated differently
        {
            // first derivation of the sigmoid function is the first part of the bias deriv
            // second part is the deriv of the activation of the layer i (h_costDeriv)
            h_biasDeriv = m_layers[i].sigmoidDeriv() * h_costDeriv; // is used again for the weights deriv and the activation deriv of the layer i-1
            m_biasDerivVelocity[i - 1] = h_biasDeriv;
        }
        else
        {
            h_biasDeriv = h_costDeriv;
            m_biasDerivVelocity[i - 1] = h_costDeriv;
        }

        // loop through the weight matrix and get the respectiv activation value that the weight caused
        // j is the index of the neuron it is going to and k is the index of the neuron it is comeing from
        h_costDeriv = Layer(m_weights[i - 1].cols(), 0);

        for (int j = 0; j < m_weights[i - 1].rows(); j++)
        {
            for (int k = 0; k < m_weights[i - 1].cols(); k++)
            {
                m_weightsDerivVelocity[i - 1].at(j, k) = h_biasDeriv.at(j) * m_layers[i - 1].at(k);

                h_costDeriv.at(k) += h_biasDeriv.at(j) * m_weights[i - 1].at(j, k);
            }
        }


        // apply the changes to the weight and bias
        m_weights[i - 1] -= (m_weightsDerivVelocity[i - 1] * m_learnFactor);
        //m_weightsDerivVelocity[i - 1] = m_weightsDerivVelocity[i - 1] * m_learnFactor;  // weight velocity is calculated as a moving average typ of thing

        m_bias[i - 1] -= (m_biasDerivVelocity[i - 1] * m_learnFactor);
        //m_biasDerivVelocity[i - 1] = m_biasDerivVelocity[i - 1] * m_learnFactor;
    }
}
