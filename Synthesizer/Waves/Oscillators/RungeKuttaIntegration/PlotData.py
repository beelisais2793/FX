#!/usr/bin/env python3



import sys
import numpy as np
import matplotlib.pyplot as plt



class PlotDuffingData:
    def __init__( self , fname ):
        self.data  =  np.loadtxt( fname )
        self.Statistics()



    def Statistics( self ):
        self.Mean =  np.mean( self.data[ : , 1: ] , axis = 0 )
        self.Var  =  np.var( self.data[ : , 1: ] , axis = 0 )


    def PlotStatistics( self ):
        plt.plot( self.Mean )
        plt.plot( self.Var )
        plt.show()



    def PlotData( self ):
        ratio = self.data.shape[0] / self.data.shape[1]
        plt.imshow( self.data[ : , 1: ] , aspect = 1/ratio )
        plt.show()















if __name__=="__main__":
    x  =  PlotDuffingData( sys.argv[1] )
    x.PlotStatistics()
    x.PlotData()
